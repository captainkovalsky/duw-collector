#include "collector.h"

#include <chrono>
#include <iomanip>
#include <optional>
#include <ranges>
#include <sstream>
#include <thread>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "../services/database_service.h"
#include "../services/env_service.h"
#include "../services/github_service.h"
#include "../services/http_client.h"

namespace {

std::string GetCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);
  
  std::ostringstream oss;
  oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

}  // anonymous namespace

namespace duw {

const std::string DUW_URL =
    "https://rezerwacje.duw.pl/status_kolejek/query.php?status";

std::optional<std::vector<TicketInfo>> ParseJsonResponse(
    const std::string& json_data) {
  if (json_data.empty()) {
    return std::nullopt;
  }

  auto json = nlohmann::json::parse(json_data, nullptr, false);
  if (json.is_discarded() || !json.contains("result")) {
    return std::nullopt;
  }

  const auto& result_obj = json["result"];
  if (!result_obj.is_object()) {
    return std::nullopt;
  }

  std::string timestamp = GetCurrentTimestamp();
  std::vector<TicketInfo> tickets;

  for (const auto& [city_name, city_data] : result_obj.items()) {
    if (!city_data.is_array() || city_data.empty()) {
      continue;
    }

    const auto& first_service = city_data[0];
    if (!first_service.is_object()) {
      continue;
    }

    auto operations_count = 0;
    auto enabled_operations = 0;

    if (first_service.contains("operations") && first_service["operations"].is_array()) {
      const auto& operations = first_service["operations"];
      operations_count = static_cast<int>(operations.size());
      
      enabled_operations = std::ranges::count_if(operations, 
        [](const auto& op) { return op.is_object() && op.value("enabled", false); });
    }

    tickets.emplace_back(TicketInfo{
      .id = 0,
      .city = city_name,
      .queue_status = "active",
      .queue_length = static_cast<int>(city_data.size()),
      .timestamp = timestamp,
      .service_name = first_service.value("name", ""),
      .service_id = first_service.value("id", -1),
      .operations_count = operations_count,
      .enabled_operations = enabled_operations
    });
  }

  return tickets.empty() ? std::nullopt : std::make_optional(std::move(tickets));
}

Collector::Collector(std::unique_ptr<HttpClient> http_client,
                     std::unique_ptr<DatabaseService> storage,
                     std::unique_ptr<EnvService> env_service,
                     std::unique_ptr<GitHubService> github_service)
    : http_client_(std::move(http_client)),
      storage_(std::move(storage)),
      env_service_(std::move(env_service)),
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
  auto params = env_service_->GetParams();
  polling_rate_seconds_ = params.polling_rate_seconds;
  
  if (!params.github_repo.empty()) {
    std::string github_db_path = params.github_repo + "/main/duw_data.db";
    if (!github_service_->FetchDatabase(github_db_path, params.db_path)) {
      spdlog::warn("Failed to fetch database from GitHub, using local database");
    } else {
      spdlog::info("Successfully fetched database from GitHub");
    }
  }
  
  return storage_->Initialize(params.db_path);
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


bool Collector::ProcessAndSaveData(const std::string& json_data) {
  auto tickets_opt = ParseJsonResponse(json_data);

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
  auto params = env_service_->GetParams();
  if (params.github_repo.empty()) {
    return;
  }
  
  std::string github_db_path = params.github_repo + "/main/duw_data.db";
  std::string commit_message = "Update DUW data - " + 
      std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
  
  if (github_service_->PushDatabase(github_db_path, params.db_path, commit_message)) {
    spdlog::info("Successfully pushed changes to GitHub");
  } else {
    spdlog::error("Failed to push changes to GitHub");
  }
}

}  // namespace duw