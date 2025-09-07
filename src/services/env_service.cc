#include "env_service.h"

#include <charconv>
#include <cstdlib>

#include <spdlog/spdlog.h>

namespace duw {

EnvServiceParams EnvService::GetParams() const {
  EnvServiceParams params;
  
  if (HasEnvVar("DB_PATH")) {
    params.db_path = GetEnvVar("DB_PATH");
  }
  
  if (HasEnvVar("GITHUB_REPO")) {
    params.github_repo = GetEnvVar("GITHUB_REPO");
  }
  
  if (HasEnvVar("POLLING_RATE_SECONDS")) {
    auto value = GetEnvVar("POLLING_RATE_SECONDS");
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), params.polling_rate_seconds);
    
    if (ec != std::errc{} || ptr != value.data() + value.size()) {
      Panic("Environment variable 'POLLING_RATE_SECONDS' has invalid integer value: " + value);
    }
  }
  
  return params;
}

std::string EnvService::GetRequiredString(const std::string& name) {
  if (!HasEnvVar(name)) {
    Panic("Required environment variable not found: " + name);
  }
  return GetEnvVar(name);
}

int EnvService::GetRequiredInt(const std::string& name) {
  if (!HasEnvVar(name)) {
    Panic("Required environment variable not found: " + name);
  }

  auto value = GetEnvVar(name);
  int result;
  auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

  if (ec != std::errc{} || ptr != value.data() + value.size()) {
    Panic("Invalid integer value for environment variable '" + name + "': " + value);
  }

  return result;
}

void EnvService::Panic(const std::string& message) {
  ExitWithError("PANIC: " + message);
}

std::string EnvService::GetEnvVar(const std::string& name) {
  const char* value = std::getenv(name.c_str());
  return (value != nullptr) ? std::string(value) : std::string();
}

bool EnvService::HasEnvVar(const std::string& name) {
  return std::getenv(name.c_str()) != nullptr;
}

void EnvService::ExitWithError(const std::string& message) {
  spdlog::critical("{}", message);
  std::exit(1);
}

}  // namespace duw