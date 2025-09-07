#include "db_connection.h"

#include <memory>

#include <spdlog/spdlog.h>
#include <sqlite3.h>

namespace duw {

DBConnection::DBConnection(const std::string& db_path) 
    : db_(nullptr, sqlite3_close) {
  sqlite3* raw_db = nullptr;
  int result_code = sqlite3_open(db_path.c_str(), &raw_db);
  if (result_code != SQLITE_OK) {
    spdlog::error("Cannot open database: {}", sqlite3_errmsg(raw_db));
    sqlite3_close(raw_db);
    db_.reset();
  } else {
    db_.reset(raw_db);
  }
}

}  // namespace duw
