#include <cstdlib>
#include <memory>

#include <spdlog/spdlog.h>
#include "collector.h"
#include "http_client.h"
#include "database_service.h"
#include "json_parser.h"
#include "env_service.h"
#include "github_service.h"

int main() {
  auto http_client = std::make_unique<duw::HttpClient>();
  auto storage = std::make_unique<duw::DatabaseService>();
  auto parser = std::make_unique<duw::JsonParser>();
  auto config = std::make_unique<duw::EnvService>();
  auto github_service = duw::CreateGitHubService(std::make_unique<duw::HttpClient>());
  
  auto collector = std::make_unique<duw::Collector>(
      std::move(http_client), std::move(storage), std::move(parser), 
      std::move(config), std::move(github_service));

  const char* mode_env = std::getenv("MODE");
  bool polling_mode =
      (mode_env != nullptr) && std::string(mode_env) == "polling";

  int result = collector->Start(polling_mode);

  if (result != 0) {
    spdlog::error("Collector failed with code: {}", result);
    return result;
  }

  if (polling_mode) {
    collector->Stop();
  }
  return 0;
}