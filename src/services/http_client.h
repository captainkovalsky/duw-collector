#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <memory>
#include <string>

#include <curl/curl.h>

namespace duw {

class HttpClient {
 public:
  HttpClient();
  ~HttpClient() = default;

  HttpClient(const HttpClient&) = delete;
  HttpClient& operator=(const HttpClient&) = delete;

  HttpClient(HttpClient&& other) noexcept = default;
  HttpClient& operator=(HttpClient&& other) noexcept = default;

  std::string Get(const std::string& url);
  std::string Put(const std::string& url, const std::string& data);

 private:
  struct CurlDeleter {
    void operator()(void* curl) const {
      if (curl) {
        curl_easy_cleanup(static_cast<CURL*>(curl));
      }
    }
  };
  
  std::unique_ptr<void, CurlDeleter> curl_;
  static size_t WriteCallback(void* contents,
                              size_t size,
                              size_t nmemb,
                              std::string* userp);
};

}  // namespace duw

#endif  // HTTP_CLIENT_H
