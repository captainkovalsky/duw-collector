#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <memory>
#include <string>
#include "../data/db_connection.h"

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

class DatabaseService {
 public:
  DatabaseService();
  ~DatabaseService() = default;

  bool Initialize(const std::string& db_path);
  bool SaveTicketInfo(const TicketInfo& ticket);

 private:
  std::unique_ptr<DBConnection> connection_;

  bool ExecuteQuery(const std::string& query);
  bool CreateTables();
  bool MigrateSchema();
};

}  // namespace duw

#endif  // DATABASE_SERVICE_H