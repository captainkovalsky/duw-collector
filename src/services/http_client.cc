#include "http_client.h"

#include <spdlog/spdlog.h>

namespace duw {

HttpClient::HttpClient() {
  // Constructor - no initialization needed since we create clients per request
}

std::string HttpClient::Get(const std::string& url) {
  httplib::Client cli(url);
  auto res = cli.Get("/");
  return res ? res->body : "";
}

std::string HttpClient::Put(const std::string& url, const std::string& data) {
  std::string response;
  
  // Parse URL to extract host and path
  size_t protocol_end = url.find("://");
  if (protocol_end == std::string::npos) {
    spdlog::error("Invalid URL format: {}", url);
    return response;
  }
  
  std::string protocol = url.substr(0, protocol_end);
  size_t host_start = protocol_end + 3;
  size_t path_start = url.find('/', host_start);
  
  std::string host = url.substr(host_start, path_start - host_start);
  std::string path = (path_start == std::string::npos) ? "/" : url.substr(path_start);
  
  // Create a new client for this specific host
  httplib::Client cli(protocol == "https" ? "https://" + host : host);
  cli.set_read_timeout(30);
  cli.set_write_timeout(30);
  cli.set_connection_timeout(10);
  cli.enable_server_certificate_verification(true);
  cli.set_follow_location(true);
  
  // Set User-Agent to avoid potential blocking
  cli.set_default_headers({
    {"User-Agent", "duw-collector/1.0"}
  });
  
  auto res = cli.Put(path, data, "application/json");
  if (!res) {
    spdlog::error("HTTP PUT request failed for URL: {} - Error: {}", url, static_cast<int>(res.error()));
    return response;
  }
  
  if (res->status != 200) {
    spdlog::error("HTTP PUT error: {} for URL: {}", res->status, url);
    return response;
  }
  
  return res->body;
}

}  // namespace duw
