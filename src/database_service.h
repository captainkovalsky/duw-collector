#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <memory>
#include "data_storage.h"
#include "db_connection.h"

namespace duw {

class DatabaseService : public DataStorage {
 public:
  DatabaseService();
  ~DatabaseService() override = default;

  bool Initialize(const std::string& db_path) override;
  bool SaveTicketInfo(const TicketInfo& ticket) override;

 private:
  std::unique_ptr<DBConnection> connection_;

  bool ExecuteQuery(const std::string& query);
  bool CreateTables();
  bool MigrateSchema();
};

}  // namespace duw

#endif  // DATABASE_SERVICE_H