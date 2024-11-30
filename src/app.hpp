#include <vector>
#include <string>
#include <raylib.h>
#include "dbi.hpp"

namespace App {
  enum UIEvent { UI_NONE, UI_HOVER_INVIS, UI_HOVER, UI_HOLD, UI_CLICK };
  enum MouseState { MOUSE_NONE, MOUSE_OVER, MOUSE_DOWN, MOUSE_HOLD, MOUSE_UP };
  class UIButtonBase {
    public:
      int id = 0;
      std::string displayTxt = "";
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      Font font = GetFontDefault();
      float fontSize = 18.0f;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = GRAY;
      Color btnDownColor = Color { 100, 100, 100, 255 };
      Color txtColor = BLACK;
      UIEvent update(MouseState mouseState);
      void render();
    private:
      Vector2 _txtPos { 0.0f, 0.0f };
      Color _activeColor = LIGHTGRAY;
  };
  class UIBox {
    public:
      int id = 0;
      std::string title = "";
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      Font font = GetFontDefault();
      float titleFontSize = 18.0f;
      float bodyFontSize = 16.0f;
      bool renderShadow = true;
      bool renderBorder = true;
      Color boxColor = GRAY;
      Color boxHoverColor = Color { 160, 160, 160, 255 };
      Color shadowColor = Color { 0, 0, 0, 50 };
      Color borderColor = BLACK;
      Color txtColor = BLACK;
      UIEvent update(MouseState mouseState, UIEvent prevUIEvent);
      void render();
    private:
      Color _activeColor = LIGHTGRAY;
  };
  class EavBlueprint: public UIButtonBase {
    public:
      EavBlueprint(DbI::EavItem item, Rectangle posSizeIn, Font fontIn) {
        id = item.blueprint_id;
        displayTxt = item.blueprint;
        posSize = posSizeIn;
        font = fontIn;
        Vector2 txtDim = MeasureTextEx(font, displayTxt.c_str(), fontSize, 0.0);
        if (txtDim.x > posSize.width) posSize.width = txtDim.x + 10.0f;
        if (txtDim.y > posSize.height) posSize.height = txtDim.y + 10.0f;
      }
  };
  class EavEntity: public UIBox {
    public:
      EavEntity(DbI::EavItem item, Rectangle posSizeIn, Font fontIn, DbI::DbInterface* dbi) {
        id = item.entity_id;
        title = item.entity;
        posSize = posSizeIn;
        font = fontIn;
        Vector2 txtDim = MeasureTextEx(font, title.c_str(), titleFontSize, 0.0);
        if (txtDim.x > posSize.width) posSize.width = txtDim.x + 10.0f;
        if (txtDim.y > posSize.height) posSize.height = txtDim.y + 10.0f;
        // fetch values
        if (dbi == NULL) return;
        DbI::EavResponse vRes = dbi->get_entity_values(item.entity_id);
        int r = GetRandomValue(150, 220);
        int g = GetRandomValue(150, 220);
        int b = GetRandomValue(120, 200);
        boxColor = Color { (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
        boxHoverColor = Color { (unsigned char)(r + 20), (unsigned char)(g + 20), (unsigned char)b, 255 };
        if (vRes.code == 0) values = vRes.data;
      }
      std::vector<DbI::EavItem> values;
      void render() {
        UIBox::render();
        for (int i=0; i<values.size(); i++) {
          DbI::EavItem v = values[i];
          std::string str = v.attr + ": ";
          switch (v.value_type) {
            case DbI::INT:
              str += std::to_string(v.int_value);
              if (v.value_unit != "") str += " " + v.value_unit; 
              break;
            case DbI::FLOAT:
              str += std::to_string(v.float_value);
              if (v.value_unit != "") str += " " + v.value_unit; 
              break;
            case DbI::BOOL:
              str += v.bool_value ? "Yes" : "No";
              break;
            default:
              str += v.str_value == "" ? "-" : v.str_value;
              break;
          }
          Vector2 pos = { posSize.x + 5.0f, posSize.y + 30 + i*bodyFontSize };
          DrawTextEx(font, str.c_str(), pos, bodyFontSize - 2.0f, 0.0f, txtColor);
        }
      };
  };
  class EventLoop {
    public:
      // global states
      int screenW = 0;
      int screenH = 0;
      Vector2 screenCenter = { 0.0f, 0.0f };
      Vector2 mousePos = { 0.0f, 0.0f };
      int fps = 0;
      double elapsed = 0.0;
      Font font;
      Color bgColor = Color { 25, 20, 30, 255 };
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
      void _updateSystem();
      void _drawFps();
  };
}