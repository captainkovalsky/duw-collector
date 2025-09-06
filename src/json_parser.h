#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "data_parser.h"

namespace duw {

class JsonParser : public DataParser {
 public:
  JsonParser() = default;
  ~JsonParser() override = default;

  std::optional<std::vector<TicketInfo>> ParseResponse(
      const std::string& json_data) override;

 private:
  static std::string GetCurrentTimestamp();
};

}  // namespace duw

#endif  // JSON_PARSER_H