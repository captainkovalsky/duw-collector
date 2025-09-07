#ifndef GITHUB_SERVICE_H
#define GITHUB_SERVICE_H

#include <memory>
#include <string>

namespace duw {

class HttpClient;
class GitHubService;

class GitHubService {
 public:
  virtual ~GitHubService() = default;
  virtual bool FetchDatabase(const std::string& repo_path, 
                           const std::string& local_path) = 0;
  virtual bool PushDatabase(const std::string& repo_path,
                           const std::string& local_path,
                           const std::string& commit_message) = 0;
  virtual bool CloseIssue(const std::string& repo_path, int issue_number) = 0;
};

std::unique_ptr<GitHubService> CreateGitHubService(std::unique_ptr<HttpClient> http_client);

}  // namespace duw

#endif  // GITHUB_SERVICE_H
