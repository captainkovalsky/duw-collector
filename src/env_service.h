#ifndef ENV_SERVICE_H
#define ENV_SERVICE_H

#include "config_service.h"

namespace duw {

class EnvService : public ConfigService {
 public:
  EnvService() = default;
  ~EnvService() override = default;

  std::string GetString(const std::string& name,
                        const std::string& defaultValue) override;
  int GetInt(const std::string& name, int defaultValue) override;

  static std::string GetRequiredString(const std::string& name);
  static int GetRequiredInt(const std::string& name);
  static void Panic(const std::string& message);

 private:
  static std::string GetEnvVar(const std::string& name);
  static bool HasEnvVar(const std::string& name);
  static void ExitWithError(const std::string& message);
};

}  // namespace duw

#endif  // ENV_SERVICE_H