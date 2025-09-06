#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <optional>
#include <string>
#include <vector>

namespace duw {

struct TicketInfo;

class DataParser {
 public:
  virtual ~DataParser() = default;
  virtual std::optional<std::vector<TicketInfo>> ParseResponse(
      const std::string& json_data) = 0;
};

}  // namespace duw

#endif  // DATA_PARSER_H
