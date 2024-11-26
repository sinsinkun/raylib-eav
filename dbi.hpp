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
    int code = 0;
    std::string msg = "";
    T data;
  };
  class BlobUtil {
    public:
      sqlite3_blob from_string(std::string s);
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
      DbResponse<int> new_value(int entityId, int attrId, void* blob, int blobSize);
      // fetch entries
      EavResponse get_blueprints();
      EavResponse get_blueprint_entities(int id);
      EavResponse get_entity_values(int id);
    private:
      unsigned int _now();
      bool _row_exists(EavItemType type, int id);
      DbResponse<int> _exec(std::string query);
      EavResponse _exec_get_eav(std::string query, EavItemType type);
  };
}