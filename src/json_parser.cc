#include "json_parser.h"

#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "data_storage.h"

namespace duw {

std::optional<std::vector<TicketInfo>> JsonParser::ParseResponse(
    const std::string& json_data) {
  std::vector<TicketInfo> tickets;

  if (json_data.empty()) {
    return std::nullopt;
  }

  nlohmann::json json;

  auto result = nlohmann::json::parse(json_data, nullptr, false);
  if (result.is_discarded()) {
    return std::nullopt;
  }

  json = result;

  if (!json.contains("result")) {
    return std::nullopt;
  }

  const auto& result_obj = json["result"];
  if (!result_obj.is_object()) {
    return std::nullopt;
  }

  std::string timestamp = GetCurrentTimestamp();

  for (const auto& [city_name, city_data] : result_obj.items()) {
    if (!city_data.is_array() || city_data.empty()) {
      continue;
    }

    int total_tickets = static_cast<int>(city_data.size());

    const auto& first_service = city_data[0];
    if (!first_service.is_object()) {
      continue;
    }

    TicketInfo ticket;
    ticket.id = 0;
    ticket.city = city_name;
    ticket.queue_status = "active";
    ticket.queue_length = total_tickets;
    ticket.timestamp = timestamp;

    ticket.service_name = first_service.value("name", "");
    ticket.service_id = first_service.value("id", -1);

    int operations_count = 0;
    int enabled_operations = 0;

    if (first_service.contains("operations") &&
        first_service["operations"].is_array()) {
      const auto& operations = first_service["operations"];
      operations_count = static_cast<int>(operations.size());

      for (const auto& operation : operations) {
        if (operation.is_object() && operation.value("enabled", false)) {
          enabled_operations++;
        }
      }
    }

    ticket.operations_count = operations_count;
    ticket.enabled_operations = enabled_operations;

    tickets.push_back(ticket);
  }

  if (tickets.empty()) {
    return std::nullopt;
  }

  return tickets;
}

std::string JsonParser::GetCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream timestamp_stream;
  timestamp_stream << std::put_time(std::localtime(&time_t),
                                    "%Y-%m-%d %H:%M:%S");
  return timestamp_stream.str();
}

}  // namespace duw