#ifndef ENV_SERVICE_H
#define ENV_SERVICE_H

#include <string>

namespace duw {

struct EnvServiceParams {
  std::string db_path = "duw_data.db";
  std::string github_repo = "";
  int polling_rate_seconds = 5;
};

class EnvService {
 public:
  EnvService() = default;
  ~EnvService() = default;

  EnvServiceParams GetParams() const;

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