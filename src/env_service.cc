#include "env_service.h"

#include <cstdlib>

#include <spdlog/spdlog.h>

namespace duw {

constexpr int BASE_10 = 10;

std::string EnvService::GetString(const std::string& name,
                                  const std::string& defaultValue) {
  if (!HasEnvVar(name)) {
    return defaultValue;
  }
  return GetEnvVar(name);
}

int EnvService::GetInt(const std::string& name, int defaultValue) {
  if (!HasEnvVar(name)) {
    return defaultValue;
  }
  std::string value = GetEnvVar(name);

  char* end;
  long result = std::strtol(value.c_str(), &end, BASE_10);

  if (*end != '\0' || end == value.c_str()) {
    Panic("Environment variable '" + name +
          "' has invalid integer value: " + value);
  }

  return static_cast<int>(result);
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

  std::string value = GetEnvVar(name);
  char* end;
  long result = std::strtol(value.c_str(), &end, BASE_10);

  if (*end != '\0' || end == value.c_str()) {
    Panic("Invalid integer value for environment variable '" + name +
          "': " + value);
  }

  return static_cast<int>(result);
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