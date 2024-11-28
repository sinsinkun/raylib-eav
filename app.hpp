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
      }
      int id = 0;
      std::string name = "";
      bool relativeToCenter = true;
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 50.0f };
      Font font = GetFontDefault();
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = GRAY;
      Color btnDownColor = Color { 100, 100, 100, 255 };
      Color txtColor = BLACK;
      float fontSize = 18.0f;
      UIEvent render();
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