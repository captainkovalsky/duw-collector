#include "http_client.h"

#include <curl/curl.h>

#include <spdlog/spdlog.h>

namespace duw {

const long HTTP_OK = 200;

HttpClient::HttpClient() {
  CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (res != CURLE_OK) {
    spdlog::critical("Failed to initialize CURL: {}", curl_easy_strerror(res));
    curl_ = nullptr;
    return;
  }

  curl_ = curl_easy_init();
  if (curl_ == nullptr) {
    spdlog::critical("Failed to create CURL handle");
    curl_global_cleanup();
  }
}

HttpClient::~HttpClient() {
  if (curl_ != nullptr) {
    curl_easy_cleanup(static_cast<CURL*>(curl_));
  }
  curl_global_cleanup();
}

HttpClient::HttpClient(HttpClient&& other) noexcept
    : curl_(other.curl_) {
  other.curl_ = nullptr;
}

HttpClient& HttpClient::operator=(HttpClient&& other) noexcept {
  if (this != &other) {
    if (curl_ != nullptr) {
      curl_easy_cleanup(static_cast<CURL*>(curl_));
    }
    curl_ = other.curl_;
    other.curl_ = nullptr;
  }
  return *this;
}

std::string HttpClient::Get(const std::string& url) {
  std::string response;
  
  if (curl_ == nullptr) {
    spdlog::error("CURL not initialized");
    return response;
  }
  
  CURL* curl = static_cast<CURL*>(curl_);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    spdlog::error("CURL request failed: {}", curl_easy_strerror(res));
    return response;
  }

  long responseCode;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
  if (responseCode != HTTP_OK) {
    spdlog::error("HTTP error: {}", responseCode);
    return response;
  }

  return response;
}

std::string HttpClient::Put(const std::string& url, const std::string& data) {
  std::string response;
  
  if (curl_ == nullptr) {
    spdlog::error("CURL not initialized");
    return response;
  }
  
  CURL* curl = static_cast<CURL*>(curl_);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    spdlog::error("CURL PUT request failed: {}", curl_easy_strerror(res));
    return response;
  }

  long responseCode;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
  if (responseCode != HTTP_OK) {
    spdlog::error("HTTP PUT error: {}", responseCode);
    return response;
  }

  return response;
}

std::string HttpClient::Patch(const std::string& url, const std::string& data) {
  std::string response;
  
  if (curl_ == nullptr) {
    spdlog::error("CURL not initialized");
    return response;
  }
  
  CURL* curl = static_cast<CURL*>(curl_);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  
  if (res != CURLE_OK) {
    spdlog::error("CURL PATCH request failed: {}", curl_easy_strerror(res));
    return response;
  }

  long responseCode;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
  if (responseCode != HTTP_OK) {
    spdlog::error("HTTP PATCH error: {}", responseCode);
    return response;
  }

  return response;
}

size_t HttpClient::WriteCallback(void* contents,
                                size_t size,
                                size_t nmemb,
                                std::string* userp) {
  size_t totalSize = size * nmemb;
  userp->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

}  // namespace duw
