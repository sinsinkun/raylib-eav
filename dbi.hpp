#include <vector>
#include <string>
#include <sqlite3.h>

namespace DbI {
  enum EavValueType { NONE, INT, FLOAT, STR, BOOL };
  enum EavItemType { UNKNOWN, BLUEPRINT, ENTITY, ATTR, BA_LINK, VALUE, VIEW };
  struct EavItem {
    EavItemType type = EavItemType::UNKNOWN;
    int blueprint_id = 0;
    int entity_id = 0;
    int attr_id = 0;
    int ba_id = 0;
    int value_id = 0;
    int created_at = 0;
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
  template <typename T>
  struct DbResponse {
    DbResponse(T d) {
      data = d;
    }
    int errCode = 0;
    std::string msg = "";
    T data;
  };
  class DbInterface {
    public:
      // parameters
      bool connected = false;
      std::string db_path = "main.db";
      sqlite3* db;
      // general functions
      void init();
      void check_tables();
      void setup_tables();
      void disconnect();
      // create new db entries
      DbResponse<int> new_blueprint(std::string name);
      DbResponse<int> new_entity(std::string name);
      DbResponse<int> new_entity(std::string name, int blueprintId);
      DbResponse<int> new_attr(std::string name, EavValueType valueType, bool allowMultiple);
      DbResponse<int> new_attr(std::string name, EavValueType valueType, bool allowMultiple, std::string unit);
      DbResponse<int> new_ba_link(int blueprintId, int attrId);
      DbResponse<int> new_value(int entityId, int attrId, std::string str_value);
      DbResponse<int> new_value(int entityId, int attrId, int int_value);
      DbResponse<int> new_value(int entityId, int attrId, float float_value);
      DbResponse<int> new_value(int entityId, int attrId, bool bool_value);
      // fetch entries
      DbResponse<std::vector<EavItem>> get_blueprints();
      DbResponse<EavItem> get_blueprint(int id);
      DbResponse<std::vector<EavItem>> get_entities();
      DbResponse<std::vector<EavItem>> get_entities(int blueprintId);
      DbResponse<EavItem> get_entity(int id);
      DbResponse<std::vector<EavItem>> get_attrs();
      DbResponse<std::vector<EavItem>> get_attrs(int blueprintId);
      DbResponse<EavItem> get_attr(int id);
    private:
      unsigned int _now();
      int _exec(std::string query);
      int _exec_stmt(std::string query, sqlite3_stmt* stmt);
      int _exec_get_eav(std::string query, EavItemType type, std::vector<EavItem>* items);
  };
}