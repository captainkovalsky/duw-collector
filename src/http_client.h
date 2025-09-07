#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

namespace duw {

class HttpClient {
 public:
  HttpClient();
  ~HttpClient();

  HttpClient(const HttpClient&) = delete;
  HttpClient& operator=(const HttpClient&) = delete;

  HttpClient(HttpClient&& other) noexcept;
  HttpClient& operator=(HttpClient&& other) noexcept;

  std::string Get(const std::string& url);
  std::string Put(const std::string& url, const std::string& data);
  std::string Patch(const std::string& url, const std::string& data);

 private:
  void* curl_ = nullptr;
  static size_t WriteCallback(void* contents,
                              size_t size,
                              size_t nmemb,
                              std::string* userp);
};

}  // namespace duw

#endif  // HTTP_CLIENT_H
