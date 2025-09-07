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
  bool CloseIssue(const std::string& repo_path, int issue_number) override;

 private:
  std::unique_ptr<HttpClient> http_client_;
  std::string BuildRawUrl(const std::string& repo_path);
  bool WriteFile(const std::string& path, const std::string& content);
  std::string ReadFile(const std::string& path);
  std::string EncodeBase64(const std::string& data);
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
  
  std::string api_url = "https://api.github.com/repos/" + repo_path;
  std::string json_payload = R"({"message":")" + commit_message + R"(","content":")" + 
                            EncodeBase64(content) + R"("})";
  
  std::string response = http_client_->Put(api_url, json_payload);
  
  if (response.empty()) {
    spdlog::error("Failed to push database to GitHub");
    return false;
  }
  
  spdlog::info("Successfully pushed database to GitHub");
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

std::string GitHubServiceImpl::EncodeBase64(const std::string& data) {
  const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string result;
  int val = 0, valb = -6;
  
  for (unsigned char c : data) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      result.push_back(chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  
  if (valb > -6) {
    result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }
  
  while (result.size() % 4) {
    result.push_back('=');
  }
  
  return result;
}

bool GitHubServiceImpl::CloseIssue(const std::string& repo_path, int issue_number) {
  std::string api_url = "https://api.github.com/repos/" + repo_path + "/issues/" + 
                        std::to_string(issue_number);
  std::string json_payload = R"({"state":"closed"})";
  
  std::string response = http_client_->Patch(api_url, json_payload);
  
  if (response.empty()) {
    spdlog::error("Failed to close GitHub issue #{}", issue_number);
    return false;
  }
  
  spdlog::info("Successfully closed GitHub issue #{}", issue_number);
  return true;
}

std::unique_ptr<GitHubService> CreateGitHubService(std::unique_ptr<HttpClient> http_client) {
  return std::make_unique<GitHubServiceImpl>(std::move(http_client));
}

}  // namespace duw
