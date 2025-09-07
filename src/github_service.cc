#include "github_service.h"

#include <fstream>
#include <sstream>

#include <spdlog/spdlog.h>

#include "http_client.h"

namespace duw {

class GitHubServiceImpl : public GitHubService {
 public:
  explicit GitHubServiceImpl(std::unique_ptr<HttpClient> http_client);
  ~GitHubServiceImpl() override = default;

  bool FetchDatabase(const std::string& repo_path, 
                    const std::string& local_path) override;
  bool PushDatabase(const std::string& repo_path,
                   const std::string& local_path,
                   const std::string& commit_message) override;

 private:
  std::unique_ptr<HttpClient> http_client_;
  std::string BuildRawUrl(const std::string& repo_path);
  bool WriteFile(const std::string& path, const std::string& content);
  std::string ReadFile(const std::string& path);
};

GitHubServiceImpl::GitHubServiceImpl(std::unique_ptr<HttpClient> http_client)
    : http_client_(std::move(http_client)) {}

bool GitHubServiceImpl::FetchDatabase(const std::string& repo_path, 
                                     const std::string& local_path) {
  std::string url = BuildRawUrl(repo_path);
  std::string content = http_client_->Get(url);
  
  if (content.empty()) {
    spdlog::error("Failed to fetch database from GitHub");
    return false;
  }
  
  return WriteFile(local_path, content);
}

bool GitHubServiceImpl::PushDatabase(const std::string& repo_path,
                                   const std::string& local_path,
                                   const std::string& commit_message) {
  std::string content = ReadFile(local_path);
  if (content.empty()) {
    spdlog::error("Failed to read local database file");
    return false;
  }
  
  spdlog::info("Database content size: {} bytes", content.size());
  spdlog::info("Would push to: {}", repo_path);
  spdlog::info("Commit message: {}", commit_message);
  
  return true;
}

std::string GitHubServiceImpl::BuildRawUrl(const std::string& repo_path) {
  return "https://raw.githubusercontent.com/" + repo_path;
}

bool GitHubServiceImpl::WriteFile(const std::string& path, const std::string& content) {
  std::ofstream file(path, std::ios::binary);
  if (!file.is_open()) {
    spdlog::error("Failed to open file for writing: {}", path);
    return false;
  }
  
  file << content;
  file.close();
  
  if (file.fail()) {
    spdlog::error("Failed to write to file: {}", path);
    return false;
  }
  
  return true;
}

std::string GitHubServiceImpl::ReadFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    spdlog::error("Failed to open file for reading: {}", path);
    return "";
  }
  
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  
  return buffer.str();
}

std::unique_ptr<GitHubService> CreateGitHubService(std::unique_ptr<HttpClient> http_client) {
  return std::make_unique<GitHubServiceImpl>(std::move(http_client));
}

}  // namespace duw
