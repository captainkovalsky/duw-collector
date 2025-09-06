#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <string>
#include <vector>

namespace duw {

struct TicketInfo {
  int id;
  std::string city;
  std::string queue_status;
  int queue_length;
  std::string timestamp;
  std::string service_name;
  int service_id;
  int operations_count;
  int enabled_operations;
};

class DataStorage {
 public:
  virtual ~DataStorage() = default;
  virtual bool Initialize(const std::string& db_path) = 0;
  virtual bool SaveTicketInfo(const TicketInfo& ticket) = 0;
};

}  // namespace duw

#endif  // DATA_STORAGE_H
