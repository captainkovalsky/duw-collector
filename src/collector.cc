#include "collector.h"

#include <chrono>
#include <thread>

#include <spdlog/spdlog.h>
#include "data_storage.h"
#include "config_service.h"
#include "http_client.h"
#include "data_parser.h"
#include "github_service.h"

namespace duw {

const std::string DUW_URL =
    "https://rezerwacje.duw.pl/status_kolejek/query.php?status";

Collector::Collector(std::unique_ptr<HttpClient> http_client,
                     std::unique_ptr<DataStorage> storage,
                     std::unique_ptr<DataParser> parser,
                     std::unique_ptr<ConfigService> config,
                     std::unique_ptr<GitHubService> github_service)
    : http_client_(std::move(http_client)),
      storage_(std::move(storage)),
      parser_(std::move(parser)),
      config_(std::move(config)),
      github_service_(std::move(github_service)) {}

Collector::~Collector() = default;

int Collector::Start(bool polling_mode) {
  if (!Initialize()) {
    spdlog::critical("Failed to initialize collector");
    return 1;
  }

  if (running_) {
    spdlog::warn("Collector is already running");
    return 0;
  }

  running_ = true;

  if (polling_mode) {
    RunPollingLoop();
  } else {
    CollectData();
  }

  return 0;
}

void Collector::Stop() {
  if (!running_) {
    spdlog::warn("Collector is not running");
    return;
  }

  running_ = false;
  PushChangesToGitHub();
}

bool Collector::IsRunning() const {
  return running_;
}

bool Collector::Initialize() {
  LoadConfiguration();

  std::string db_path = config_->GetString("DB_PATH", "duw_data.db");
  std::string github_repo = config_->GetString("GITHUB_REPO", "");
  
  if (!github_repo.empty()) {
    std::string github_db_path = github_repo + "/main/duw_data.db";
    if (!github_service_->FetchDatabase(github_db_path, db_path)) {
      spdlog::warn("Failed to fetch database from GitHub, using local database");
    } else {
      spdlog::info("Successfully fetched database from GitHub");
    }
  }
  
  return storage_->Initialize(db_path);
}

void Collector::CollectData() {
  std::string duwData = FetchDuwData();
  if (duwData.empty() || !ValidateData(duwData)) {
    spdlog::critical("Failed to collect DUW data");
    running_ = false;
    return;
  }

  if (!ProcessAndSaveData(duwData)) {
    spdlog::critical("Failed to process and save data");
    running_ = false;
    return;
  }

  running_ = false;
}

std::string Collector::FetchDuwData() {
  std::string response = http_client_->Get(DUW_URL);
  if (response.empty()) {
    spdlog::critical("Empty response from DUW API");
  }
  return response;
}

void Collector::LoadConfiguration() {
  polling_rate_seconds_ = config_->GetInt("POLLING_RATE_SECONDS",
                                         Collector::DEFAULT_POLLING_RATE);
}

bool Collector::ProcessAndSaveData(const std::string& json_data) {
  auto tickets_opt = parser_->ParseResponse(json_data);

  if (!tickets_opt.has_value()) {
    spdlog::error("JSON parsing failed");
    return false;
  }

  auto tickets = tickets_opt.value();
  if (tickets.empty()) {
    spdlog::warn("No tickets found in response");
    return false;
  }

  int saved_count = 0;
  for (const auto& ticket : tickets) {
    if (storage_->SaveTicketInfo(ticket)) {
      saved_count++;
    } else {
      spdlog::error("Failed to save ticket for city: {}", ticket.city);
    }
  }

  return saved_count > 0;
}

void Collector::RunPollingLoop() {
  while (running_) {
    CollectData();

    if (!running_) {
      break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(polling_rate_seconds_));
  }
}

bool Collector::ValidateData(const std::string& data) {
  return !data.empty();
}

void Collector::PushChangesToGitHub() {
  std::string github_repo = config_->GetString("GITHUB_REPO", "");
  if (github_repo.empty()) {
    return;
  }
  
  std::string db_path = config_->GetString("DB_PATH", "duw_data.db");
  std::string github_db_path = github_repo + "/main/duw_data.db";
  std::string commit_message = "Update DUW data - " + 
      std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
  
  if (github_service_->PushDatabase(github_db_path, db_path, commit_message)) {
    spdlog::info("Successfully pushed changes to GitHub");
  } else {
    spdlog::error("Failed to push changes to GitHub");
  }
}

}  // namespace duw