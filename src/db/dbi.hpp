#pragma once

#include <vector>
#include <string>
#include <sqlite3.h>

namespace DbI {
  enum EavValueType { NONE, INT, FLOAT, STR, BOOL };
  enum EavItemType { UNKNOWN, BLUEPRINT, ENTITY, ATTR, BA_LINK, VALUE, VIEW };
  enum QueryComparator { ENTITY_NAMED, BP_ID, ATTR_LIKE, ATTR_EQUAL, ATTR_GT, ATTR_GTE, ATTR_LT, ATTR_LTE, ATTR_NULL };
  enum QueryChain { Q_AND, Q_OR };
  struct EavItem {
    EavItemType type = EavItemType::UNKNOWN;
    int blueprint_id = 0;
    int entity_id = 0;
    int attr_id = 0;
    int ba_id = 0;
    int value_id = 0;
    unsigned int created_at = 0;
    std::string blueprint = "";
    std::string entity = "";
    std::string attr = "";
    std::string value_unit = "";
    bool allow_multiple = false;
    EavValueType value_type = EavValueType::NONE;
    std::string str_value = "";
    int int_value = 0;
    float float_value = 0.0;
    bool bool_value = false;
  };
  struct EntityQuery {
    public:
      QueryComparator comparator = ENTITY_NAMED;
      QueryChain chain = Q_AND;
      int id = 0;
      std::string entity;
      std::string attr;
      std::string value;
  };
  template <typename T>
  struct DbResponse {
    DbResponse(T d) {
      data = d;
    }
    int code = 0;
    std::string msg = "";
    T data;
  };
  typedef DbResponse<std::vector<EavItem>> EavResponse;
  class DbInterface {
    public:
      // parameters
      bool connected = false;
      std::string db_path = "main.db";
      sqlite3* db;
      // general functions
      void init();
      int check_tables();
      void setup_tables();
      void disconnect();
      // create new db entries
      DbResponse<int> new_blueprint(std::string name);
      DbResponse<int> new_entity(std::string name);
      DbResponse<int> new_entity(std::string name, int blueprintId);
      DbResponse<int> new_attr(std::string name, EavValueType valueType, bool allowMultiple);
      DbResponse<int> new_attr(std::string name, EavValueType valueType, bool allowMultiple, std::string unit);
      DbResponse<int> new_ba_link(int blueprintId, int attrId);
      DbResponse<int> new_attr_for_blueprint(int blueprintId, std::string name, EavValueType valueType, bool allowMultiple, std::string unit);
      DbResponse<int> new_value(int entityId, int attrId, std::string value);
      // fetch entries
      DbResponse<EavItem> get_one(EavItemType type, int id);
      EavResponse get_blueprints();
      EavResponse get_blueprint_attrs(int id);
      EavResponse get_blueprint_entities(int id);
      EavResponse get_attrs();
      EavResponse get_entity_values(int id);
      EavResponse search_entities(std::vector<EntityQuery> queries);
      EavResponse get_values_like(int attrId, std::string query);
      // update entries
      DbResponse<int> update_blueprint(int id, std::string name);
      DbResponse<int> update_entity(int id, int blueprintId, std::string name);
      DbResponse<int> update_attr(int id, std::string name, EavValueType valueType, bool allowMultiple);
      DbResponse<int> update_attr(int id, std::string name, EavValueType valueType, bool allowMultiple, std::string unit);
      DbResponse<int> update_value(int id, std::string value);
      // delete entries
      DbResponse<int> delete_any(EavItemType type, int id);
      DbResponse<int> delete_all_attr_links(int attrId);
      DbResponse<int> delete_all_attr_values(int attrId);
      DbResponse<int> delete_all_entity_values(int entityId);
    private:
      unsigned int _now();
      bool _row_exists(EavItemType type, int id);
      DbResponse<int> _last_inserted_id();
      DbResponse<int> _exec(std::string query);
      EavResponse _exec_get_eav(std::string query, EavItemType type);
  };
  // util functions
  std::string value_type_to_str(EavValueType type);
  EavValueType str_to_value_type(std::string str);
  bool isValidDecimal(std::string str);
  bool isValidInteger(std::string str);
}