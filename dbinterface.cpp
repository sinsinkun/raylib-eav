#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <sqlite3.h>
#include "app.hpp"

using namespace App;

// connect to db
void DbInterface::init() {
  std::cout << "Opening db at: " + db_path << std::endl;
  int rcode = sqlite3_open(db_path.c_str(), &db);
  std::string srcode = std::to_string(rcode);
  if (rcode) {
    sqlite3_close(db);
    std::cout << "Error: Could not open sqlite db (" + srcode + ")" << std::endl;
  } else {
    std::cout << "Opened " + db_path + " (" + srcode + ")" << std::endl;
  }
}

// get time since epoch in ms
unsigned int DbInterface::_now() {
  auto instant = std::chrono::system_clock::now();
  auto duration = instant.time_since_epoch();
  return duration.count();
}

// generic query execute with no return value
int DbInterface::_exec(std::string query) {
  char* errMsg;
  int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errMsg);
  if (rc != SQLITE_OK) {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError1 - " << db_error_msg << std::endl;
  }
  return rc;
}

int DbInterface::_exec_get_eav(std::string query, EavItemType type, std::vector<EavItem>* items) {
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, NULL);
  if (rc != SQLITE_OK) {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError - " << db_error_msg << std::endl;
    return 1;
  }
  // convert sqlite data into eav item
  rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
    int cc = sqlite3_column_count(stmt);
    // EavItem item;
    // item.type = type;
    for (int i=0; i < cc; i++) {
      const char* col = sqlite3_column_name(stmt, i);
      const unsigned char* v = sqlite3_column_text(stmt, i);
      std::cout << "column (" << i << "/" << cc << ") " << col << ": " << v << std::endl;
    }
    rc = sqlite3_step(stmt);
  }
  return 0;
}

// resets database tables. WARNING: ERASES ALL DATA - CANNOT BE UNDONE
void DbInterface::setup_tables() {
  // delete old tables
  _exec("DROP TABLE IF EXISTS eav_schemas;");
  _exec("DROP TABLE IF EXISTS eav_entities;");
  _exec("DROP TABLE IF EXISTS eav_attrs;");
  _exec("DROP TABLE IF EXISTS eav_sa_links;");
  _exec("DROP TABLE IF EXISTS eav_values;");

  // create eav_schemas
  std::string et_create = "CREATE TABLE eav_schemas (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "entity_type TEXT UNIQUE NOT NULL);";
  _exec(et_create);

  // create eav_entities
  std::string e_create = "CREATE TABLE eav_entities (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "entity TEXT NOT NULL, " \
    "entity_type_id INTEGER UNIQUE NOT NULL);";
  _exec(e_create);

  // create eav_attrs
  std::string a_create = "CREATE TABLE eav_attrs (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
	  "attr TEXT UNIQUE NOT NULL, " \
    // defines what kind of value is stored in this attr (e.g. str, int, float, bool, blob)
	  "value_type TEXT NOT NULL, " \
    // optionally define a unit for the attribute
	  "value_unit TEXT, " \
    // defines if multiple entries are allowed per entity
	  "allow_multiple BOOLEAN NOT NULL);";
  _exec(a_create);

  // create eav_sa_links
  std::string sa_create = "CREATE TABLE eav_sa_links (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "schema_id INTEGER NOT NULL, "
    "attr_id INTEGER NOT NULL);";
  _exec(sa_create);

  // create eav_values
  std::string v_create = "CREATE TABLE eav_values (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "entity_id INTEGER NOT NULL, " \
    "attr_id INTEGER NOT NULL);";
  _exec(v_create);

  std::cout << "Finished database setup" << std::endl;
}

void DbInterface::new_schema(std::string name) {
  std::string query = "INSERT INTO eav_schemas (entity_type, created_at) VALUES (\"";
  std::string now = std::to_string(_now());
  query += name + "\"," + now + ");";
  int ec = _exec(query);
  if (ec == SQLITE_OK) {
    std::cout << "Created new entity type" << std::endl;
  }
}

void DbInterface::new_entity(std::string name, int entityTypeId) {
  std::string query = "INSERT INTO eav_entities (entity, entity_type_id, created_at) VALUES (\"";
  std::string etid = std::to_string(entityTypeId);
  std::string now = std::to_string(_now());
  query += name + "\"," + etid + "," + now + ");";
  int ec = _exec(query);
  if (ec == SQLITE_OK) {
    std::cout << "Created new entity" << std::endl;
  }
}

void DbInterface::get_tables() {
  std::string query = "SELECT name FROM sqlite_master WHERE TYPE = 'table';";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, NULL);
  if (rc == SQLITE_OK) {
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
      // int cc = sqlite3_column_count(stmt);
      // const char* c = sqlite3_column_name(stmt, 0);
      const unsigned char* v = sqlite3_column_text(stmt, 0);
      std::cout << "found row: " << v << std::endl;
      rc = sqlite3_step(stmt);
    }
  } else {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError - " << db_error_msg << std::endl;
  }
  rc = sqlite3_finalize(stmt);
  if (rc == SQLITE_OK) {
    std::cout << "Finished fetching tables" << std::endl;
  } else {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "Sqlite3 Error - " << db_error_msg << std::endl;
  }
}

void DbInterface::get_schemas() {
  std::string query = "SELECT * FROM eav_schemas;";
  std::vector<EavItem> items;
  int rc = _exec_get_eav(query, EavItemType::SCHEMA, &items);
  std::cout << "Got entity types" << std::endl;
}

void DbInterface::disconnect() {
  int rcode = sqlite3_close(db);
  std::string srcode = std::to_string(rcode);
  std::cout << "Closed db (" + srcode + ")" << std::endl;
}