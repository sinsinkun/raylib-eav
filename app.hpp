#include <vector>
#include <string>
#include <raylib.h>
#include <sqlite3.h>

namespace App {
  enum EavValueType { NONE, INT, FLOAT, STR, BOOL, BLOB };
  enum EavItemType { UNKNOWN, BLUEPRINT, ENTITY, ATTR, VALUE, VIEW };
  class EavItem {
    public:
      EavItemType type = EavItemType::UNKNOWN;
      int schema_id = 0;
      int entity_id = 0;
      int attr_id = 0;
      int value_id = 0;
      int created_at = 0;
      std::string schema = "";
      std::string entity = "";
      std::string attr = "";
      std::string value_unit = "";
      bool allow_multiple = false;
      EavValueType value_type = EavValueType::NONE;
      std::string str_value;
      int int_value;
      float float_value;
      bool bool_value;
      unsigned char* blob_value;
  };
  class DbInterface {
    public:
      // parameters
      bool connected = false;
      std::string db_path = "main.db";
      sqlite3* db;
      // general functions
      void init();
      void setup_tables();
      void disconnect();
      // create new db entries
      void new_blueprint(std::string name);
      void new_entity(std::string name, int blueprintId);
      void new_attr(std::string name, EavValueType valueType, bool allowMultiple);
      void new_attr(std::string name, EavValueType valueType, bool allowMultiple, std::string unit);
      void new_eta_link(int blueprintId, int attrId);
      void new_value(int entityId, int attrId, std::string str_value);
      void new_value(int entityId, int attrId, int int_value);
      void new_value(int entityId, int attrId, float float_value);
      // fetch entries
      void get_tables();
      void get_blueprints();
    private:
      unsigned int _now();
      int _exec(std::string query);
      int _exec_stmt(std::string query, sqlite3_stmt* stmt);
      int _exec_get_eav(std::string query, EavItemType type, std::vector<EavItem>* items);
  };
  class EventLoop {
    public:
      // global states
      int screenW = 0;
      int screenH = 0;
      Vector2 screenCenter = { 0.0, 0.0 };
      int fps = 0;
      double elapsed = 0.0;
      DbInterface dbInterface;
      // methods
      void init();
      void update();
      void render();
      void cleanup();
    private:
      void _updateSystem();
      void _drawFps();
  };
}