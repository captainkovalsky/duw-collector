#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

#include <string>

namespace duw {

class ConfigService {
 public:
  virtual ~ConfigService() = default;
  virtual std::string GetString(const std::string& name,
                                const std::string& defaultValue) = 0;
  virtual int GetInt(const std::string& name, int defaultValue) = 0;
};

}  // namespace duw

#endif  // CONFIG_SERVICE_H
