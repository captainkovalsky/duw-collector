#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <memory>
#include <string>

#include <httplib.h>

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
  // No need to store client since we create it per request
};

}  // namespace duw

#endif  // HTTP_CLIENT_H
