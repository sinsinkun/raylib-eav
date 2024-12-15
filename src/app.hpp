#pragma once

#include <vector>
#include <string>
#include <raylib.h>
#include "db/dbi.hpp"
#include "ui/ui.hpp"

namespace App {
  // specific use
  enum DialogOption {
    NO_ACTION, NEW_BLUEPRINT, NEW_ENTITY, NEW_ATTR, NEW_VALUE, EDIT_BLUEPRINT, 
    EDIT_ENTITY, DEL_BLUEPRINT, DEL_ATTR, DEL_ENTITY, DEL_VALUE
  };
  enum OptionsParent { OP_NONE, OP_BLUEPRINT, OP_ATTR, OP_ENTITY };
  class OptionsMenu {
    public:
      OptionsMenu() {};
      OptionsMenu(UIState* globalState, OptionsParent parent);
      bool isVisible = false;
      int blueprintId = 0;
      int entityId = 0;
      int attrId = 0;
      int valueId = 0;
      std::string metaText = "";
      OptionsParent parent = OP_NONE;
      UIBox box = UIBox(NULL);
      UIButton btn1 = UIButton(NULL);
      UIButton btn2 = UIButton(NULL);
      UIButton btn3 = UIButton(NULL);
      void open();
      int update();
      void render();
  };
  class EavBlueprint {
    public:
      EavBlueprint(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn);
      int id = 0;
      std::string name = "";
      bool isActive = false;
      UIButton btn = UIButton(NULL);
      bool update();
      void render();
  };
  class EavEntity {
    public:
      EavEntity(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn, DbI::DbInterface* dbi);
      int blueprintId = 0;
      int id = 0;
      std::string name = "";
      std::vector<DbI::EavItem> values;
      UIBox box = UIBox(NULL);
      void refreshBody(DbI::DbInterface* db);
      void fillBody();
      bool update();
      void render();
  };
  class ErrorBox {
    public:
      ErrorBox(UIState* globalState) { state = globalState; }
      UIState* state = NULL;
      std::string msg;
      void setError(std::string err) {
        msg = err;
        _timer = 0.01f;
      };
      void update() {
        if (_timer > 0.0f) _timer += GetFrameTime();
        if (_timer> 2.5f) {
          _timer = 0.0f;
          msg.clear();
        }
      };
      void render() {
        if (msg.empty() || state == NULL) return;
        DrawTextEx(state->font, msg.c_str(), Vector2{10.0f, 10.0f}, 18.0f, 0.0f, RED);
      };
    private:
      float _timer = 0.0f;
  };
  class AppBar {
    public:
      AppBar() {};
      AppBar(UIState* globalState, Rectangle bounds);
      UIBox box = UIBox(NULL);
      UIInput searchInput = UIInput(NULL);
      UIButton searchBtn = UIButton(NULL);
      std::vector<UIButton> categoryBtns;
      int update();
      void render();
      void cleanup();
  };
  class EnhancedInput : public UIInput {
    public:
      EnhancedInput(UIState* globalState) : UIInput(globalState) {};
      EnhancedInput(UIState* globalState, Rectangle bounds);
      EnhancedInput(
        UIState* globalState,
        Rectangle bounds,
        std::string label,
        int attrId,
        DbI::EavValueType valueType,
        int valueId
      );
      int attrId = 0;
      int valueId = 0;
      float botMargin = 5.0f;
      DbI::EavValueType valueType = DbI::NONE;
      std::string label;
      void updatePos(float boxLeft, float yOffset);
      void render();
  };
  class AttrRadio : public UIRadio {
    public:
      AttrRadio(UIState* globalState) : UIRadio(globalState) {};
      AttrRadio(UIState* gs, Vector2 tl, std::string txt) : UIRadio(gs, tl, txt) {};
      AttrRadio(
        UIState* globalState,
        Vector2 topLeft,
        int attrId,
        std::string attr,
        DbI::EavValueType valueType,
        bool allowMultiple,
        std::string unit,
        int baId
      );
      int attrId = 0;
      int baId = 0;
      std::string attr;
      DbI::EavValueType valueType = DbI::NONE;
      bool allowMultiple = false;
      std::string unit;
  };
  class SideBar {
    public:
      SideBar() {};
      SideBar(UIState* globalState, DbI::DbInterface* db, Rectangle screenBounds);
      DbI::DbInterface* db = NULL;
      bool open = false;
      DialogOption action = NO_ACTION;
      int blueprintId = 0;
      int entityId = 0;
      int attrId = 0;
      int valueId = 0;
      UIBox box = UIBox(NULL);
      UIButton closeBtn = UIButton(NULL);
      UIButton btn1 = UIButton(NULL);
      UIButton btn2 = UIButton(NULL);
      std::vector<EnhancedInput> inputs;
      std::vector<AttrRadio> radios;
      void changeDialog(DialogOption action, std::string metaText,
        int blueprintId, int entityId, int attrId, int valueId);
      int update();
      void render();
      void cleanup();
    private:
      void clearInputs();
      int inputStarti = 1;
      int inputEndi = 0;
      int radioStarti = 0;
      int radioEndi = 0;
      float scrollDelta = 0.0f;
      float yLimit = 100.0f;
      bool showScroll = false;
  };
  class EventLoop {
    public:
      // global states
      Color bgColor = Color { 35, 35, 40, 255 };
      // ui objects
      UIState uiGlobal;
      OptionsMenu menu;
      ErrorBox errBox = ErrorBox(NULL);
      AppBar appBar;
      SideBar sideBar;
      // data objects
      DbI::DbInterface dbInterface;
      std::vector<EavBlueprint> categories;
      std::vector<EavEntity> entities;
      // methods
      void init();
      void update();
      void render();
      void cleanup(); 
    private:
      void _drawFps();
      // db actions
      void _fetchAllCategories();
      void _fillEntities(DbI::EavResponse* res);
      void _fetchCategory(int blueprintId);
      void _multisearch(std::string query);
      DbI::EavResponse _search(std::string query);
      // helpers
      void _handleSideBar(SideBar* sideBar);
      void _handleOption(OptionsMenu* menu, int action);
  };
  // util functions
  std::vector<std::string> str_split(std::string str, std::string delimiter);
  std::vector<std::string> str_split_length(std::string str, int size);
  std::string trim_space(std::string str);
  bool isValidDecimal(std::string str);
  bool isValidInteger(std::string str);
}