#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <memory>
#include <string>

namespace duw {

class HttpClient;
class EnvService;
class DatabaseService;
class GitHubService;

class Collector {
 public:
  static constexpr int DEFAULT_POLLING_RATE = 5;

  Collector(std::unique_ptr<HttpClient> http_client,
            std::unique_ptr<DatabaseService> storage,
            std::unique_ptr<EnvService> env_service,
            std::unique_ptr<GitHubService> github_service);
  ~Collector();
  int Start(bool polling_mode = false);
  void Stop();
  bool IsRunning() const;

 private:
  bool running_ = false;
  std::unique_ptr<HttpClient> http_client_;
  std::unique_ptr<DatabaseService> storage_;
  std::unique_ptr<EnvService> env_service_;
  std::unique_ptr<GitHubService> github_service_;
  int polling_rate_seconds_ = DEFAULT_POLLING_RATE;
  bool Initialize();
  void CollectData();
  static bool ValidateData(const std::string& data);
  std::string FetchDuwData();
  void LoadConfiguration();
  bool ProcessAndSaveData(const std::string& json_data);
  void RunPollingLoop();
  void PushChangesToGitHub();
};

}  // namespace duw

#endif  // COLLECTOR_H