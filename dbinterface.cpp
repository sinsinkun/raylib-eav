#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <sqlite3.h>
#include "dbi.hpp"

using namespace DbI;

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

#pragma region helpers
// get time since epoch in ms
unsigned int DbInterface::_now() {
  auto instant = std::chrono::system_clock::now();
  auto duration = instant.time_since_epoch();
  return duration.count();
}

std::string _value_type_to_str(EavValueType type) {
  switch (type) {
    case EavValueType::STR: return "str";
    case EavValueType::INT: return "int";
    case EavValueType::BOOL: return "bool";
    case EavValueType::FLOAT: return "float";
    default: return "null";
  }
}

EavValueType _str_to_value_type(std::string str) {
  if (str == "str") return EavValueType::STR;
  else if (str == "int") return EavValueType::INT;
  else if (str == "bool") return EavValueType::BOOL;
  else if (str == "float") return EavValueType::FLOAT;
  else return EavValueType::NONE;
}

// generic query execute with no return value
DbResponse<int> DbInterface::_exec(std::string query) {
  DbResponse<int> res = DbResponse(0);
  char* errMsg;
  int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errMsg);
  if (rc != SQLITE_OK) {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError - " << db_error_msg << std::endl;
    res.msg = db_error_msg;
  } else {
    res.msg = "OK";
  }
  res.code = rc;
  return res;
}

// query execute with EavItem parsing for return value
EavResponse DbInterface::_exec_get_eav(std::string query, EavItemType type) {
  std::vector<EavItem> items;
  EavResponse res = DbResponse(items);
  sqlite3_stmt* stmt;
  // send query to sqlite3
  int rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, NULL);
  if (rc != SQLITE_OK) {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError - " << db_error_msg << std::endl;
    res.code = rc;
    res.msg = db_error_msg;
    return res;
  }
  // convert sqlite data into eav item
  rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
    int cc = sqlite3_column_count(stmt);
    EavItem item;
    item.type = type;
    for (int i=0; i < cc; i++) {
      const char* col = sqlite3_column_name(stmt, i);
      std::string colstr = col;
      if (colstr == "id") {
        int id = sqlite3_column_int(stmt, i);
        switch (type) {
          case BLUEPRINT:
            item.blueprint_id = id;
            break;
          case ENTITY:
            item.entity_id = id;
            break;
          case ATTR:
            item.attr_id = id;
            break;
          case BA_LINK:
            item.ba_id = id;
            break;
          case VALUE:
            item.value_id = id;
            break;
          default:
            break;
        }
      } else if (colstr == "blueprint_id") {
        item.blueprint_id = sqlite3_column_int(stmt, i);
      } else if (colstr == "entity_id") {
        item.entity_id = sqlite3_column_int(stmt, i);
      } else if (colstr == "attr_id") {
        item.attr_id = sqlite3_column_int(stmt, i);
      } else if (colstr == "ba_id") {
        item.ba_id = sqlite3_column_int(stmt, i);
      } else if (colstr == "value_id") {
        item.value_id = sqlite3_column_int(stmt, i);
      } else if (colstr == "created_at") {
        int ca = sqlite3_column_int(stmt, i);
        item.created_at = ca;
      } else if (colstr == "blueprint") {
        item.blueprint = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
      } else if (colstr == "entity") {
        item.entity = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
      } else if (colstr == "attr") {
        item.attr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
      } else if (colstr == "value_unit") {
        const unsigned char* vu = sqlite3_column_text(stmt, i);
        if (vu != NULL) item.value_unit = reinterpret_cast<const char*>(vu);
      } else if (colstr == "allow_multiple") {
        bool am = sqlite3_column_int(stmt, i);
        item.allow_multiple = am;
      } else if (colstr == "value_type") {
        std::string vt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
        item.value_type = _str_to_value_type(vt);
      } else if (colstr == "value") {
        // note: value_type MUST be interpreted before value
        const unsigned char* sv = sqlite3_column_text(stmt, i);
        sqlite3_value* v = sqlite3_column_value(stmt, i);
        float dv = sqlite3_value_double(v);
        int iv = sqlite3_value_int(v);
        switch (item.value_type) {
          case STR:
            if (sv != NULL) {
              item.str_value = reinterpret_cast<const char*>(sv);
            }
            break;
          case INT:
            item.int_value = sqlite3_column_int(stmt, i);
            break;
          case FLOAT:
            item.float_value = sqlite3_column_double(stmt, i);
            break;
          case BOOL:
            item.bool_value = sqlite3_column_int(stmt, i);
            break;
          default:
            break;
        }
      }
    }
    items.push_back(item);
    rc = sqlite3_step(stmt);
  }
  // clean up
  res.data = items;
  rc = sqlite3_finalize(stmt);
  res.code = rc;
  if (rc != SQLITE_OK) {
    std::string db_error_msg = sqlite3_errmsg(db);
    res.msg = db_error_msg;
  } else {
    res.msg = "OK";
  }
  return res;
}

// check if id for type exists
bool DbInterface::_row_exists(EavItemType type, int id) {
  std::string table = "";
  switch (type) {
    case BLUEPRINT:
      table = "eav_blueprints";
      break;
    case ENTITY:
      table = "eav_entities";
      break;
    case ATTR:
      table = "eav_attrs";
      break;
    case BA_LINK:
      table = "eav_ba_links";
      break;
    case VALUE:
      table = "eav_values";
      break;
    default:
      break;
  }
  if (table == "") {
    std::cout << "ERR: Could not find table from type" << type << std::endl;
    return false;
  }
  std::string query = "SELECT COUNT(*) FROM " + table + " WHERE id = " + std::to_string(id) + ";";
  sqlite3_stmt* stmt;
  // send query to sqlite3
  int rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, NULL);
  if (rc != SQLITE_OK) {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError - " << db_error_msg << std::endl;
    return false;
  }
  // convert sqlite data into eav item
  rc = sqlite3_step(stmt);
  bool out = false;
  if (rc == SQLITE_ROW) {
    int count = sqlite3_column_int(stmt, 0);
    if (count == 1) out = true;
  }
  // clean up
  sqlite3_finalize(stmt);
  return out;
}

// resets database tables. WARNING: ERASES ALL DATA - CANNOT BE UNDONE
void DbInterface::setup_tables() {
  // delete old tables
  _exec("DROP TABLE IF EXISTS eav_blueprints;");
  _exec("DROP TABLE IF EXISTS eav_entities;");
  _exec("DROP TABLE IF EXISTS eav_attrs;");
  _exec("DROP TABLE IF EXISTS eav_ba_links;");
  _exec("DROP TABLE IF EXISTS eav_values;");

  // create eav_blueprints
  std::string b_create = "CREATE TABLE eav_blueprints (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "blueprint TEXT UNIQUE NOT NULL);";
  _exec(b_create);

  // create eav_entities
  std::string e_create = "CREATE TABLE eav_entities (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "entity TEXT NOT NULL, " \
    "blueprint_id INTEGER);";
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

  // create eav_ba_links
  std::string ba_create = "CREATE TABLE eav_ba_links (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "blueprint_id INTEGER NOT NULL, "
    "attr_id INTEGER NOT NULL);";
  _exec(ba_create);

  // create eav_values
  std::string v_create = "CREATE TABLE eav_values (" \
    "id INTEGER PRIMARY KEY, " \
    // created_at time as milliseconds from epoch
    "created_at INTEGER, " \
    "entity_id INTEGER NOT NULL, " \
    "attr_id INTEGER NOT NULL, " \
    "value BLOB);";
  _exec(v_create);

  std::cout << "Finished database setup" << std::endl;
}

int DbInterface::check_tables() {
  std::string query = "SELECT name FROM sqlite_master WHERE TYPE = 'table';";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, NULL);
  int sum = 0;
  if (rc == SQLITE_OK) {
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW) {
      std::string tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
      if (tableName == "eav_blueprints") sum += 11;
      else if (tableName == "eav_entities") sum += 17;
      else if (tableName == "eav_attrs") sum += 29;
      else if (tableName == "eav_ba_links") sum += 37;
      else if (tableName == "eav_values") sum += 41;
      rc = sqlite3_step(stmt);
    }
  } else {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError - " << db_error_msg << std::endl;
  }
  rc = sqlite3_finalize(stmt);
  if (rc == SQLITE_OK) {
    std::cout << "Fetched tables - ";
    if (sum == 0) std::cout << "no tables found. Rebuilding...";
    else if (sum == 135) std::cout << "all tables accounted for";
    else std::cout << "table shape malformed";
    std::cout << std::endl;
    // setup tables if no tables found
    if (sum == 0) setup_tables();
  } else {
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "Sqlite3 Error - " << db_error_msg << std::endl;
  }
  return rc;
}
#pragma endregion helpers

#pragma region new_entries
DbResponse<int> DbInterface::new_blueprint(std::string name) {
  std::string query = "INSERT INTO eav_blueprints (blueprint, created_at) VALUES (\"";
  std::string now = std::to_string(_now());
  query += name + "\"," + now + ");";
  DbResponse<int> res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::new_entity(std::string name) {
  std::string query = "INSERT INTO eav_entities (entity, created_at) VALUES (\"";
  std::string now = std::to_string(_now());
  query += name + "\"," + now + ");";
  DbResponse<int> res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::new_entity(std::string name, int blueprintId) {
  // check blueprint exists
  bool bp_exists = _row_exists(BLUEPRINT, blueprintId);
  if (!bp_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Blueprint does not exist";
    return res;
  }
  // build entity
  std::string query = "INSERT INTO eav_entities (entity, blueprint_id, created_at) VALUES (\"";
  std::string etid = std::to_string(blueprintId);
  std::string now = std::to_string(_now());
  query += name + "\"," + etid + "," + now + ");";
  DbResponse<int> res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::new_attr(std::string name, EavValueType valueType, bool allowMultiple) {
  std::string query = "INSERT INTO eav_attrs (attr, value_type, allow_multiple, created_at) VALUES (\"";
  std::string vType = _value_type_to_str(valueType);
  std::string am = std::to_string(allowMultiple);
  std::string now = std::to_string(_now());
  query += name + "\",\"" + vType + "\"," + am + "," + now + ");";
  DbResponse<int> res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::new_attr(std::string name, EavValueType valueType, bool allowMultiple, std::string unit) {
  if (valueType != EavValueType::INT && valueType != EavValueType::FLOAT) {
    std::cout << "WARN: attr of this type cannot have a unit" << std::endl;
    return SQLITE_ABORT;
  }
  std::string query = "INSERT INTO eav_attrs (attr, value_type, allow_multiple, value_unit, created_at) VALUES (\"";
  std::string vType = _value_type_to_str(valueType);
  std::string am = std::to_string(allowMultiple);
  std::string now = std::to_string(_now());
  query += name + "\",\"" + vType + "\"," + am + ",\"" + unit + "\"," + now + ");";
  DbResponse<int> res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::new_ba_link(int blueprintId, int attrId) {
  // check blueprint exists
  bool bp_exists = _row_exists(BLUEPRINT, blueprintId);
  if (!bp_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Blueprint does not exist";
    return res;
  }
  // check attribute exists
  bool attr_exists = _row_exists(ATTR, attrId);
  if (!attr_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 2;
    res.msg = "Attr does not exist";
    return res;
  }
  // build ba link
  std::string query = "INSERT INTO eav_ba_links (blueprint_id, attr_id, created_at) VALUES (";
  std::string bid = std::to_string(blueprintId);
  std::string aid = std::to_string(attrId);
  std::string now = std::to_string(_now());
  query += bid + "," + aid + "," + now + ");";
  DbResponse<int> res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::new_value(int entityId, int attrId, void* blob, int size) {
  // check entity exists
  bool e_exists = _row_exists(BLUEPRINT, entityId);
  if (!e_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Entity does not exist";
    return res;
  }
  // check attribute exists
  bool attr_exists = _row_exists(ATTR, attrId);
  if (!attr_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 2;
    res.msg = "Attr does not exist";
    return res;
  }
  // todo: check entity is allowed to have attribute
  // build value
  DbResponse<int> res = DbResponse(0);
  sqlite3_stmt* stmt;
  std::string query = "INSERT INTO eav_values (entity_id, attr_id, value, created_at) VALUES (";
  std::string eid = std::to_string(entityId);
  std::string aid = std::to_string(attrId);
  std::string now = std::to_string(_now());
  query += eid + "," + aid + ", ? ," + now + ");";
  int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    res.code = rc;
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError prepare - " << db_error_msg << std::endl;
    res.msg = db_error_msg;
    return res;
  }
  rc = sqlite3_bind_blob(stmt, 1, blob, size, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    res.code = rc;
    std::string db_error_msg = sqlite3_errmsg(db);
    std::cout << "SqliteError bind - " << db_error_msg << std::endl;
    res.msg = db_error_msg;
    return res;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    res.code = 1012;
    std::cout << "SqliteError execution - failed" << std::endl;
    res.msg = "Execution failed";
  }
  sqlite3_finalize(stmt);
  return res;
}
#pragma endregion new_entries

#pragma region fetch_entries
EavResponse DbInterface::get_blueprints() {
  std::string query = "SELECT * FROM eav_blueprints;";
  EavResponse res = _exec_get_eav(query, EavItemType::BLUEPRINT);
  return res;
}

EavResponse DbInterface::get_blueprint_entities(int id) {
  bool exists = _row_exists(BLUEPRINT, id);
  if (!exists) {
    std::vector<EavItem> vec;
    EavResponse res = DbResponse(vec);
    res.code = 1;
    res.msg = "Blueprint does not exist";
    return res;
  }
  std::string query = "SELECT b.id as blueprint_id, e.id as entity_id, b.blueprint, e.entity " \
    "FROM eav_blueprints b " \
    "LEFT JOIN eav_entities e ON e.blueprint_id = b.id " \
    "WHERE b.id = " + std::to_string(id) + ";";
  EavResponse res = _exec_get_eav(query, EavItemType::VIEW);
  return res;
}

EavResponse DbInterface::get_entity_values(int id) {
  bool exists = _row_exists(ENTITY, id);
  if (!exists) {
    std::vector<EavItem> vec;
    EavResponse res = DbResponse(vec);
    res.code = 1;
    res.msg = "Entity does not exist";
    return res;
  }
  std::string query = "SELECT eb.id as blueprint_id, eb.blueprint, ee.id as entity_id, ee.entity, " \
    "ea.id as attr_id, ea.attr, ea.value_type, ea.value_unit, ea.allow_multiple, " \
    "ev.id as value_id, ev.value "
    "FROM eav_blueprints eb " \
    "LEFT JOIN eav_entities ee ON ee.blueprint_id = eb.id " \
    "LEFT JOIN eav_ba_links ebl ON ebl.blueprint_id = eb.id " \
    "LEFT JOIN eav_attrs ea ON ebl.attr_id = ea.id " \
    "LEFT JOIN eav_values ev ON ev.entity_id = ee.id AND ev.attr_id = ea.id " \
    "WHERE ee.id = " + std::to_string(id) + " ORDER BY blueprint_id, entity_id, attr_id;";
  EavResponse res = _exec_get_eav(query, EavItemType::VIEW);
  return res;
}
#pragma endregion fetch_entries

void DbInterface::disconnect() {
  int rcode = sqlite3_close(db);
  std::string srcode = std::to_string(rcode);
  std::cout << "Closed db (" + srcode + ")" << std::endl;
}