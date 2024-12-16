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

std::string DbI::value_type_to_str(EavValueType type) {
  switch (type) {
    case EavValueType::STR: return "str";
    case EavValueType::INT: return "int";
    case EavValueType::BOOL: return "bool";
    case EavValueType::FLOAT: return "float";
    default: return "null";
  }
}

EavValueType DbI::str_to_value_type(std::string str) {
  if (str == "str") return EavValueType::STR;
  else if (str == "int") return EavValueType::INT;
  else if (str == "bool") return EavValueType::BOOL;
  else if (str == "float") return EavValueType::FLOAT;
  else return EavValueType::NONE;
}

DbResponse<int> DbInterface::_last_inserted_id() {
  DbResponse<int> res = DbResponse(0);
  sqlite3_stmt* stmt;
  std::string query = "SELECT last_insert_rowid()";
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
  if (rc == SQLITE_ROW) {
    int v = sqlite3_column_int(stmt, 0);
    res.data = v;
    res.code = 0;
    res.msg = "OK";
  } else {
    res.code = 999;
    res.msg = "Could not find last inserted id";
  }
  sqlite3_finalize(stmt);
  return res;
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
      const unsigned char* vPtr = sqlite3_column_text(stmt, i);
      if (vPtr == NULL) {
        // skip NULL values
        continue;
      } else if (colstr == "id") {
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
        unsigned int ca = sqlite3_column_int(stmt, i);
        item.created_at = ca;
      } else if (colstr == "blueprint") {
        item.blueprint = reinterpret_cast<const char*>(vPtr);
      } else if (colstr == "entity") {
        item.entity = reinterpret_cast<const char*>(vPtr);
      } else if (colstr == "attr") {
        item.attr = reinterpret_cast<const char*>(vPtr);
      } else if (colstr == "value_unit") {
        item.value_unit = reinterpret_cast<const char*>(vPtr);
      } else if (colstr == "allow_multiple") {
        bool am = sqlite3_column_int(stmt, i);
        item.allow_multiple = am;
      } else if (colstr == "value_type") {
        std::string vt = reinterpret_cast<const char*>(vPtr);
        item.value_type = str_to_value_type(vt);
      } else if (colstr == "value") {
        // note: value_type MUST be interpreted before value
        bool conversion_failed = false;
        if (vPtr != NULL) {
          std::string strv = reinterpret_cast<const char*>(vPtr);
          item.str_value = strv;
          switch (item.value_type) {
            case INT:
              try {
                item.int_value = stoi(strv);
              } catch (...) {
                conversion_failed = true;
              }
              break;
            case FLOAT:
              try {
                item.float_value = stof(strv);
              } catch (...) {
                conversion_failed = true;
              }
              break;
            case BOOL:
              if (strv == "true" || strv == "TRUE" || strv == "1") {
                item.bool_value = true;
                item.str_value = "true";
              } else if (strv == "false" || strv == "FALSE" || strv == "0") {
                item.str_value = "false";
              }
              break;
            case STR:
            default:
              break;
          }
        }
        if (conversion_failed) {
          res.code = 91;
          res.msg = "Invalid data in value column - Could not be converted";
          return res;
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
    // comparison operators interpret text as numbers
    "value TEXT);";
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
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
  return res;
}

DbResponse<int> DbInterface::new_entity(std::string name) {
  std::string query = "INSERT INTO eav_entities (entity, created_at) VALUES (\"";
  std::string now = std::to_string(_now());
  query += name + "\"," + now + ");";
  DbResponse<int> res = _exec(query);
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
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
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
  return res;
}

DbResponse<int> DbInterface::new_attr(std::string name, EavValueType valueType, bool allowMultiple) {
  std::string query = "INSERT INTO eav_attrs (attr, value_type, allow_multiple, created_at) VALUES (\"";
  std::string vType = value_type_to_str(valueType);
  std::string am = std::to_string(allowMultiple);
  std::string now = std::to_string(_now());
  query += name + "\",\"" + vType + "\"," + am + "," + now + ");";
  DbResponse<int> res = _exec(query);
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
  return res;
}

DbResponse<int> DbInterface::new_attr(std::string name, EavValueType valueType, bool allowMultiple, std::string unit) {
  if (valueType != EavValueType::INT && valueType != EavValueType::FLOAT) {
    std::cout << "WARN: attr of this type cannot have a unit" << std::endl;
    DbResponse<int> res = DbResponse(0);
    res.code = SQLITE_ABORT;
    res.msg = "attr with this type cannot have units";
    return res;
  }
  std::string query = "INSERT INTO eav_attrs (attr, value_type, allow_multiple, value_unit, created_at) VALUES (\"";
  std::string vType = value_type_to_str(valueType);
  std::string am = std::to_string(allowMultiple);
  std::string now = std::to_string(_now());
  query += name + "\",\"" + vType + "\"," + am + ",\"" + unit + "\"," + now + ");";
  DbResponse<int> res = _exec(query);
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
  return res;
}

DbResponse<int> DbInterface::new_ba_link(int blueprintId, int attrId) {
  // check blueprint exists
  DbResponse<int> res = DbResponse(0);
  bool bp_exists = _row_exists(BLUEPRINT, blueprintId);
  if (!bp_exists) {
    res.code = 1;
    res.msg = "Blueprint does not exist";
    return res;
  }
  // check attribute exists
  bool attr_exists = _row_exists(ATTR, attrId);
  if (!attr_exists) {
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
  res = _exec(query);
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
  return res;
}

DbResponse<int> DbInterface::new_attr_for_blueprint(int blueprintId, std::string name, EavValueType valueType, bool allowMultiple, std::string unit) {
  DbResponse<int> res = DbResponse(0);
  // check if blueprint exists
  bool bp_exists = _row_exists(BLUEPRINT, blueprintId);
  if (!bp_exists) {
    res.code = 1;
    res.msg = "Blueprint does not exist";
    return res;
  }
  // check if attr exists
  std::string aquery = "SELECT * FROM eav_attrs WHERE attr = \"" + name + 
    "\" AND value_type = \"" + value_type_to_str(valueType) + "\";";
  EavResponse aRes = _exec_get_eav(aquery, ATTR);
  int attrId = 0;
  if (aRes.code == 0 && aRes.data.size() == 0) {
    // insert attr
    if (unit == "") res = new_attr(name, valueType, allowMultiple);
    else res = new_attr(name, valueType, allowMultiple, unit);
    if (res.code != SQLITE_OK) return res;
    attrId = res.data;
  } else if (aRes.code == 0 && aRes.data.size() == 1) {
    // grab existing attr id
    attrId = aRes.data[0].attr_id;
  } else {
    res.code = aRes.code;
    res.msg = aRes.msg;
    return res;
  }
  // insert bp_link
  if (res.code == SQLITE_OK && attrId != 0) {
    DbResponse<int> lres = new_ba_link(blueprintId, attrId);
    if (lres.code != SQLITE_OK) return lres;
  }
  return res;
}

// WARNING: NO TYPE CHECKING ON VALUE
DbResponse<int> DbInterface::new_value(int entityId, int attrId, std::string value) {
  DbResponse<int> res = DbResponse(0);
  // check attribute exists
  bool attr_exists = _row_exists(ATTR, attrId);
  if (!attr_exists) {
    res.code = 1;
    res.msg = "Attr does not exist";
    return res;
  }
  // check entity exists and is allowed to have attribute
  std::string equery = "SELECT * FROM eav_entities WHERE id = " + std::to_string(entityId);
  EavResponse entities = _exec_get_eav(equery, ENTITY);
  if (entities.code != SQLITE_OK) {
    res.code = entities.code;
    res.msg = entities.msg;
    return res;
  }
  if (entities.data.size() == 0) {
    res.code = 2;
    res.msg = "Entity does not exist";
    return res;
  }
  EavItem entity = entities.data.at(0);
  // check if blueprint allows attribute
  if (entity.blueprint_id != 0) {
    std::string cquery = "SELECT eb.id as blueprint_id, ea.id as attr_id, eb.blueprint, ea.attr " \
      "FROM eav_blueprints eb " \
      "LEFT JOIN eav_ba_links ebl ON ebl.blueprint_id = eb.id " \
      "LEFT JOIN eav_attrs ea ON ebl.attr_id = ea.id " \
      "WHERE eb.id = " + std::to_string(entity.blueprint_id) + " AND ea.id = " + std::to_string(attrId);
    EavResponse attrs = _exec_get_eav(cquery, VIEW);
    if (attrs.code != SQLITE_OK) {
      res.code = entities.code;
      res.msg = entities.msg;
      return res;
    }
    if (attrs.data.size() == 0) {
      res.code = 3;
      res.msg = "Blueprint does not allow attribute on this entity";
      return res;
    }
  }
  // check if entity + attr already exist, and if multiple is allowed
  std::string vquery = "SELECT ev.id as value_id, ev.attr_id, ea.allow_multiple, ea.value_type " \
  "FROM eav_values ev " \
  "LEFT JOIN eav_attrs ea ON ev.attr_id = ea.id " \
  "WHERE ev.entity_id = " + std::to_string(entityId) + " AND ev.attr_id = " + std::to_string(attrId);
  EavResponse values = _exec_get_eav(vquery, VIEW);
  if (values.code != SQLITE_OK) {
    res.code = entities.code;
    res.msg = entities.msg;
    return res;
  }
  if (values.data.size() > 0) {
    EavItem ev = values.data.at(0);
    if (!ev.allow_multiple) {
      res.code = 5;
      res.msg = "Attribute value already exists";
      return res;
    }
  }
  // build value
  std::string query = "INSERT INTO eav_values (entity_id, attr_id, value, created_at) VALUES (";
  std::string eid = std::to_string(entityId);
  std::string aid = std::to_string(attrId);
  std::string now = std::to_string(_now());
  query += eid + "," + aid + ",\"" + value + "\"," + now + ");";
  res = _exec(query);
  if (res.code != SQLITE_OK) return res;
  res = _last_inserted_id();
  return res;
}
#pragma endregion new_entries

#pragma region fetch_entries
DbResponse<EavItem> DbInterface::get_one(EavItemType type, int id) {
  EavItem item;
  DbResponse<EavItem> res = DbResponse(item);
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
    res.code = 1;
    res.msg = "Invalid table for deletion";
    return res;
  }
  std::string query = "SELECT * FROM " + table + " WHERE id = " + std::to_string(id);
  EavResponse rs = _exec_get_eav(query, type);
  if (rs.code != SQLITE_OK) {
    res.code = rs.code;
    res.msg = rs.msg;
    return res;
  }
  if (rs.data.size() == 0) {
    res.code = 1;
    res.msg = "No item found";
    return res;
  }
  res.msg = "OK";
  res.data = rs.data.at(0);
  return res;
}

EavResponse DbInterface::get_blueprints() {
  std::string query = "SELECT * FROM eav_blueprints;";
  EavResponse res = _exec_get_eav(query, EavItemType::BLUEPRINT);
  return res;
}

EavResponse DbInterface::get_blueprint_attrs(int id) {
  bool exists = _row_exists(BLUEPRINT, id);
  if (!exists) {
    std::vector<EavItem> vec;
    EavResponse res = DbResponse(vec);
    res.code = 1;
    res.msg = "Blueprint does not exist";
    return res;
  }
  std::string query = "SELECT b.id as blueprint_id, b.blueprint, ba.id as ba_id, " \
    "a.id as attr_id, a.attr, a.value_type, a.allow_multiple, a.value_unit " \
    "FROM eav_blueprints b " \
    "INNER JOIN eav_ba_links ba ON ba.blueprint_id = b.id " \
    "INNER JOIN eav_attrs a ON ba.attr_id = a.id " \
    "WHERE b.id = " + std::to_string(id);
  EavResponse res = _exec_get_eav(query, EavItemType::VIEW);
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
    "INNER JOIN eav_entities e ON e.blueprint_id = b.id " \
    "WHERE b.id = " + std::to_string(id) + ";";
  EavResponse res = _exec_get_eav(query, EavItemType::VIEW);
  return res;
}

EavResponse DbInterface::get_attrs() {
  std::string query = "SELECT * FROM eav_attrs";
  EavResponse res = _exec_get_eav(query, EavItemType::ATTR);
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
    "ebl.id as ba_id, ev.id as value_id, ev.value " \
    "FROM eav_blueprints eb " \
    "LEFT JOIN eav_entities ee ON ee.blueprint_id = eb.id " \
    "LEFT JOIN eav_ba_links ebl ON ebl.blueprint_id = eb.id " \
    "LEFT JOIN eav_attrs ea ON ebl.attr_id = ea.id " \
    "LEFT JOIN eav_values ev ON ev.entity_id = ee.id AND ev.attr_id = ea.id " \
    "WHERE ee.id = " + std::to_string(id) + " ORDER BY blueprint_id, entity_id, attr_id;";
  EavResponse res = _exec_get_eav(query, EavItemType::VIEW);
  return res;
}

EavResponse DbInterface::get_entities_like(std::string q) {
  std::string query = "SELECT * FROM eav_entities WHERE entity LIKE \"%" + q + "%\" ORDER BY blueprint_id DESC";
  EavResponse res = _exec_get_eav(query, EavItemType::ENTITY);
  return res;
}

EavResponse DbInterface::get_entities_like(std::string q, int bpId) {
  std::string query = "SELECT ee.* FROM eav_entities ee " \
    "LEFT JOIN eav_values ev ON ev.entity_id = ee.id " \
    "WHERE ee.entity LIKE \"%" + q + "%\" " \
    "AND ee.blueprint_id = " + std::to_string(bpId);
  EavResponse res = _exec_get_eav(query, EavItemType::ENTITY);
  return res;
}

EavResponse DbInterface::get_entities_attrs_like(std::string a, std::string v, std::string cmp) {
  // fetch attr
  std::string aquery = "SELECT * FROM eav_attrs WHERE attr = \"" + a + "\"";
  EavResponse aRes = _exec_get_eav(aquery, EavItemType::ATTR);
  if (aRes.code != 0) return aRes;
  if (aRes.data.empty()) {
    aRes.code = 924;
    aRes.msg = "ERR: Attribute not found";
    return aRes;
  }
  int attrId = aRes.data[0].attr_id;
  // value converters
  if (aRes.data[0].value_type == BOOL) {
    cmp = "=";
    if (v == "yes" || v == "Yes") v = "\"true\"";
    else if (v == "no" || v == "No") v = "\"false\"";
    else v = "\"" + v + "\"";
  } else if (cmp == "LIKE") {
    v = "\"%" + v + "%\"";
  } else if (aRes.data[0].value_type == STR) {
    v = "\"" + v + "\"";
  }
  // fetch entities using attr comparison
  std::string query = "SELECT ee.* FROM eav_entities ee " \
    "LEFT JOIN eav_values ev ON ev.entity_id = ee.id " \
    "WHERE ev.attr_id = " + std::to_string(attrId) + " AND ev.value " \
    + cmp + " " + v + " ORDER BY blueprint_id DESC";
  EavResponse res = _exec_get_eav(query, EavItemType::ENTITY);
  return res;
}

EavResponse DbInterface::get_entities_attrs_empty(std::string a) {
  // fetch attr
  std::string aquery = "SELECT * FROM eav_attrs WHERE attr = \"" + a + "\"";
  EavResponse aRes = _exec_get_eav(aquery, EavItemType::ATTR);
  if (aRes.code != 0) return aRes;
  if (aRes.data.empty()) {
    aRes.code = 924;
    aRes.msg = "ERR: Attribute not found: " + a;
    return aRes;
  }
  // fetch entities using attr comparison
  std::string attrId = std::to_string(aRes.data[0].attr_id);
  std::string query = "SELECT ee.* from eav_blueprints eb " \
    "INNER JOIN eav_entities ee ON eb.id = ee.blueprint_id " \
    "INNER JOIN eav_ba_links ebl ON ebl.blueprint_id = eb.id AND ebl.attr_id = " + attrId + " " \
    "LEFT JOIN eav_values ev ON ev.entity_id = ee.id AND ev.attr_id = " + attrId + " " \
    "WHERE ev.value IS NULL ORDER BY blueprint_id DESC";
  EavResponse res = _exec_get_eav(query, EavItemType::ENTITY);
  return res;
}

EavResponse DbInterface::get_values_like(int attrId, std::string q) {
  std::string query = "SELECT * FROM eav_values WHERE attr_id = " + std::to_string(attrId) + 
    " AND value LIKE \"%" + q + "%\"";
  EavResponse res = _exec_get_eav(query, EavItemType::VALUE);
  return res;
}
#pragma endregion fetch_entries

#pragma region update_entries
DbResponse<int> DbInterface::update_blueprint(int id, std::string name) {
  // check blueprint exists
  bool b_exists = _row_exists(BLUEPRINT, id);
  if (!b_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Blueprint entry does not exist";
    return res;
  }
  // build blueprint
  std::string query = "UPDATE eav_blueprints SET blueprint = \"" + name + "\" WHERE id = " + std::to_string(id);
  DbResponse<int> res = _exec(query);
  if (res.code == SQLITE_OK) res.data = id;
  return res;
}

DbResponse<int> DbInterface::update_entity(int id, int blueprintId, std::string name) {
  // check entity exists
  bool e_exists = _row_exists(ENTITY, id);
  if (!e_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Entity entry does not exist";
    return res;
  }
  // build entity
  std::string query = "UPDATE eav_entities SET entity = \"" + name + 
    "\", blueprint_id = " + std::to_string(blueprintId) + " WHERE id = " + std::to_string(id);
  DbResponse<int> res = _exec(query);
  if (res.code == SQLITE_OK) res.data = id;
  return res;
}

DbResponse<int> DbInterface::update_attr(int id, std::string name, EavValueType valueType, bool allowMultiple) {
  // check attr exists
  bool a_exists = _row_exists(ATTR, id);
  if (!a_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Entity entry does not exist";
    return res;
  }
  // build attr
  std::string vtypeStr = value_type_to_str(valueType);
  std::string am = std::to_string(allowMultiple);
  std::string query = "UPDATE eav_attrs SET attr = \"" +
    name + "\", value_type = \"" + vtypeStr + "\", allow_multiple = " + am +
    " WHERE id = " + std::to_string(id);
  DbResponse<int> res = _exec(query);
  if (res.code == SQLITE_OK) res.data = id;
  return res;
}

DbResponse<int> DbInterface::update_attr(int id, std::string name, EavValueType valueType, bool allowMultiple, std::string unit) {
  // check attr exists
  bool a_exists = _row_exists(ATTR, id);
  if (!a_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Entity entry does not exist";
    return res;
  }
  // build attr
  std::string vtypeStr = value_type_to_str(valueType);
  std::string am = std::to_string(allowMultiple);
  std::string query = "UPDATE eav_attrs SET attr = \"" +
    name + "\", value_type = \"" + vtypeStr + "\", allow_multiple = " + am + ", value_unit = " + unit +
    " WHERE id = " + std::to_string(id);
  DbResponse<int> res = _exec(query);
  if (res.code == SQLITE_OK) res.data = id;
  return res;
}

DbResponse<int> DbInterface::update_value(int id, std::string value) {
  // check value exists
  bool v_exists = _row_exists(VALUE, id);
  if (!v_exists) {
    DbResponse<int> res = DbResponse(0);
    res.code = 1;
    res.msg = "Value entry does not exist";
    return res;
  }
  // build value
  std::string query = "UPDATE eav_values SET value = \"" + value + "\" WHERE id = " + std::to_string(id);
  DbResponse<int> res = _exec(query);
  if (res.code == SQLITE_OK) res.data = id;
  return res;
}
#pragma endregion update_entries

DbResponse<int> DbInterface::delete_any(EavItemType type, int id) {
  DbResponse<int> res = DbResponse(0);
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
    res.code = 1;
    res.msg = "Invalid table for deletion";
    return res;
  }
  std::string query = "DELETE FROM " + table + " WHERE id = " + std::to_string(id);
  res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::delete_all_attr_links(int id) {
  DbResponse<int> res = DbResponse(0);
  std::string query = "DELETE FROM eav_ba_links WHERE attr_id = " + std::to_string(id);
  res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::delete_all_attr_values(int id) {
  DbResponse<int> res = DbResponse(0);
  std::string query = "DELETE FROM eav_values WHERE attr_id = " + std::to_string(id);
  res = _exec(query);
  return res;
}

DbResponse<int> DbInterface::delete_all_entity_values(int id) {
  DbResponse<int> res = DbResponse(0);
  std::string query = "DELETE FROM eav_values WHERE entity_id = " + std::to_string(id);
  res = _exec(query);
  return res;
}

void DbInterface::disconnect() {
  int rcode = sqlite3_close(db);
  std::string srcode = std::to_string(rcode);
  std::cout << "Closed db (" + srcode + ")" << std::endl;
}