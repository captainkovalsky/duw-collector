#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <memory>
#include <string>

struct sqlite3;

namespace duw {

class DBConnection {
 public:
  explicit DBConnection(const std::string& db_path);
  ~DBConnection() = default;

  DBConnection(const DBConnection&) = delete;
  DBConnection& operator=(const DBConnection&) = delete;

  DBConnection(DBConnection&& other) noexcept = default;
  DBConnection& operator=(DBConnection&& other) noexcept = default;

  sqlite3* Get() const { return db_.get(); }
  bool IsValid() const { return db_ != nullptr; }

 private:
  std::unique_ptr<sqlite3, int(*)(sqlite3*)> db_;
};

}  // namespace duw

#endif  // DB_CONNECTION_H
