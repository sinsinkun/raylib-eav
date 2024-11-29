#include <vector>
#include <string>
#include <raylib.h>
#include "dbi.hpp"

namespace App {
  enum UIEvent { NO_EVENT, BTN_HOVER, BTN_HOLD, BTN_CLICK };
  class EavBlueprint {
    public:
      EavBlueprint(DbI::EavItem item, Rectangle posSizeIn, Font fontIn) {
        id = item.blueprint_id;
        name = item.blueprint;
        posSize = posSizeIn;
        font = fontIn;
        Vector2 txtDim = MeasureTextEx(font, name.c_str(), fontSize, 0.0);
        if (txtDim.x > posSize.width) posSize.width = txtDim.x + 10.0f;
        if (txtDim.y > posSize.height) posSize.height = txtDim.y + 10.0f;
      }
      int id = 0;
      std::string name = "";
      bool relativeToCenter = true;
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 50.0f };
      Font font = GetFontDefault();
      float fontSize = 18.0f;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = GRAY;
      Color btnDownColor = Color { 100, 100, 100, 255 };
      Color txtColor = BLACK;
      bool holding = false;
      Vector2 originalMouseLock = { 0.0f, 0.0f };
      UIEvent render();
      UIEvent render(Vector2 screenCenter, Vector2 mousePos);
  };
  class EavEntity {
    public:
      EavEntity(DbI::EavItem item, Rectangle posSizeIn, Font fontIn) {
        id = item.entity_id;
        name = item.entity;
        posSize = posSizeIn;
        font = fontIn;
        Vector2 txtDim = MeasureTextEx(font, name.c_str(), fontSize, 0.0);
        if (txtDim.x > posSize.width) posSize.width = txtDim.x + 10.0f;
        if (txtDim.y > posSize.height) posSize.height = txtDim.y + 10.0f;
      }
      int id = 0;
      std::string name = "";
      bool relativeToCenter = true;
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 50.0f };
      Font font = GetFontDefault();
      float fontSize = 18.0f;
      Color btnColor = Color { 160, 160, 220, 255 };
      Color btnHoverColor = Color { 100, 100, 180, 255 };
      Color btnDownColor = Color { 60, 60, 100, 255 };
      Color txtColor = BLACK;
      bool holding = false;
      Vector2 originalMouseLock = { 0.0f, 0.0f };
      UIEvent render(Vector2 screenCenter, Vector2 mousePos);
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