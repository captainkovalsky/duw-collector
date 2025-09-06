#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <string>

struct sqlite3;

namespace duw {

class DBConnection {
 public:
  explicit DBConnection(const std::string& db_path);
  ~DBConnection();

  DBConnection(const DBConnection&) = delete;
  DBConnection& operator=(const DBConnection&) = delete;

  DBConnection(DBConnection&& other) noexcept;
  DBConnection& operator=(DBConnection&& other) noexcept;

  sqlite3* Get() const { return db_; }
  bool IsValid() const { return db_ != nullptr; }

 private:
  sqlite3* db_ = nullptr;
};

}  // namespace duw

#endif  // DB_CONNECTION_H
