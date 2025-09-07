#include "database_service.h"

#include <cstdint>
#include <cstdlib>

#include <spdlog/spdlog.h>
#include <sqlite3.h>

#include "../data/db_connection.h"

namespace duw {

enum class BindIndex : std::uint8_t {
  BIND_CITY = 1,
  BIND_QUEUE_STATUS = 2,
  BIND_QUEUE_LENGTH = 3,
  BIND_TIMESTAMP = 4,
  BIND_SERVICE_NAME = 5,
  BIND_SERVICE_ID = 6,
  BIND_OPERATIONS_COUNT = 7,
  BIND_ENABLED_OPERATIONS = 8
};

enum class ColumnIndex : std::uint8_t {
  COL_ID = 0,
  COL_CITY = 1,
  COL_QUEUE_STATUS = 2,
  COL_QUEUE_LENGTH = 3,
  COL_TIMESTAMP = 4,
  COL_SERVICE_NAME = 5,
  COL_SERVICE_ID = 6,
  COL_OPERATIONS_COUNT = 7,
  COL_ENABLED_OPERATIONS = 8
};

DatabaseService::DatabaseService() = default;

bool DatabaseService::Initialize(const std::string& db_path) {
  connection_ = std::make_unique<DBConnection>(db_path);
  
  if (!connection_->IsValid()) {
    return false;
  }

  if (!CreateTables()) {
    spdlog::error("Failed to create database tables");
    return false;
  }

  if (!MigrateSchema()) {
    spdlog::error("Failed to migrate database schema");
    return false;
  }

  return true;
}

bool DatabaseService::SaveTicketInfo(const TicketInfo& ticket) {
  const char* sql = R"(
    INSERT INTO ticket_info (city, queue_status, queue_length, timestamp, service_name, service_id, operations_count, enabled_operations)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?);
  )";

  sqlite3_stmt* stmt;
  int result_code = sqlite3_prepare_v2(connection_->Get(), sql, -1, &stmt, nullptr);
  if (result_code != SQLITE_OK) {
    spdlog::error("Failed to prepare statement: {}", sqlite3_errmsg(connection_->Get()));
    return false;
  }

  sqlite3_bind_text(stmt, static_cast<int>(BindIndex::BIND_CITY),
                    ticket.city.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, static_cast<int>(BindIndex::BIND_QUEUE_STATUS),
                    ticket.queue_status.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, static_cast<int>(BindIndex::BIND_QUEUE_LENGTH),
                   ticket.queue_length);
  sqlite3_bind_text(stmt, static_cast<int>(BindIndex::BIND_TIMESTAMP),
                    ticket.timestamp.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, static_cast<int>(BindIndex::BIND_SERVICE_NAME),
                    ticket.service_name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, static_cast<int>(BindIndex::BIND_SERVICE_ID),
                   ticket.service_id);
  sqlite3_bind_int(stmt, static_cast<int>(BindIndex::BIND_OPERATIONS_COUNT),
                   ticket.operations_count);
  sqlite3_bind_int(stmt, static_cast<int>(BindIndex::BIND_ENABLED_OPERATIONS),
                   ticket.enabled_operations);

  result_code = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (result_code != SQLITE_DONE) {
    spdlog::error("Failed to insert ticket: {}", sqlite3_errmsg(connection_->Get()));
    return false;
  }

  return true;
}

bool DatabaseService::CreateTables() {
  const char* sql = R"(
    CREATE TABLE IF NOT EXISTS ticket_info (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      city TEXT NOT NULL,
      queue_status TEXT NOT NULL,
      queue_length INTEGER NOT NULL,
      timestamp TEXT NOT NULL,
      service_name TEXT,
      service_id INTEGER,
      operations_count INTEGER DEFAULT 0,
      enabled_operations INTEGER DEFAULT 0,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    );
    
    CREATE INDEX IF NOT EXISTS idx_timestamp ON ticket_info(timestamp);
    CREATE INDEX IF NOT EXISTS idx_city ON ticket_info(city);
    CREATE INDEX IF NOT EXISTS idx_created_at ON ticket_info(created_at);
  )";

  return ExecuteQuery(sql);
}


bool DatabaseService::ExecuteQuery(const std::string& query) {
  int result_code = sqlite3_exec(connection_->Get(), query.c_str(), nullptr, nullptr, nullptr);

  if (result_code != SQLITE_OK) {
    spdlog::error("SQL error: {}", sqlite3_errmsg(connection_->Get()));
    return false;
  }

  return true;
}

bool DatabaseService::MigrateSchema() {
  const char* check_table = R"(
    SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='ticket_info';
  )";

  sqlite3_stmt* stmt;
  int result_code = sqlite3_prepare_v2(connection_->Get(), check_table, -1, &stmt, nullptr);
  if (result_code != SQLITE_OK) {
    spdlog::error("Failed to check table existence: {}", sqlite3_errmsg(connection_->Get()));
    return false;
  }

  result_code = sqlite3_step(stmt);
  bool table_exists = false;
  if (result_code == SQLITE_ROW) {
    table_exists = sqlite3_column_int(stmt, 0) > 0;
  }
  sqlite3_finalize(stmt);

  if (!table_exists) {
    return true;
  }

  const char* check_old_schema = R"(
    SELECT COUNT(*) FROM pragma_table_info('ticket_info') WHERE name='raw_json';
  )";

  result_code = sqlite3_prepare_v2(connection_->Get(), check_old_schema, -1, &stmt, nullptr);
  if (result_code != SQLITE_OK) {
    spdlog::error("Failed to check schema: {}", sqlite3_errmsg(connection_->Get()));
    return false;
  }

  result_code = sqlite3_step(stmt);
  bool has_old_schema = false;
  if (result_code == SQLITE_ROW) {
    has_old_schema = sqlite3_column_int(stmt, 0) > 0;
  }
  sqlite3_finalize(stmt);

  if (!has_old_schema) {
    return true;
  }

  const char* backup_sql = R"(
    CREATE TABLE IF NOT EXISTS ticket_info_backup AS 
    SELECT * FROM ticket_info;
  )";

  if (!ExecuteQuery(backup_sql)) {
    return false;
  }

  if (!ExecuteQuery("DROP TABLE ticket_info;")) {
    return false;
  }

  if (!CreateTables()) {
    return false;
  }

  const char* migrate_sql = R"(
    INSERT INTO ticket_info (id, city, queue_status, queue_length, timestamp, service_name, service_id, operations_count, enabled_operations, created_at)
    SELECT 
      id, 
      city, 
      queue_status, 
      queue_length, 
      timestamp, 
      'legacy_service' as service_name,
      0 as service_id,
      0 as operations_count,
      0 as enabled_operations,
      created_at
    FROM ticket_info_backup;
  )";

  if (!ExecuteQuery(migrate_sql)) {
    return false;
  }

  ExecuteQuery("DROP TABLE ticket_info_backup;");

  return true;
}

}  // namespace duw