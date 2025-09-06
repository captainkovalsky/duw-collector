#include "db_connection.h"

#include <spdlog/spdlog.h>
#include <sqlite3.h>

namespace duw {

DBConnection::DBConnection(const std::string& db_path) {
  int result_code = sqlite3_open(db_path.c_str(), &db_);
  if (result_code != SQLITE_OK) {
    spdlog::error("Cannot open database: {}", sqlite3_errmsg(db_));
    db_ = nullptr;
  }
}

DBConnection::~DBConnection() {
  if (db_ != nullptr) {
    sqlite3_close(db_);
  }
}

DBConnection::DBConnection(DBConnection&& other) noexcept
    : db_(other.db_) {
  other.db_ = nullptr;
}

DBConnection& DBConnection::operator=(DBConnection&& other) noexcept {
  if (this != &other) {
    if (db_ != nullptr) {
      sqlite3_close(db_);
    }
    db_ = other.db_;
    other.db_ = nullptr;
  }
  return *this;
}

}  // namespace duw
