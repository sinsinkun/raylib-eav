#include <vector>
#include <string>
#include <raylib.h>
#include "dbi.hpp"

namespace App {
  class UIButton {
    public:
      UIButton(int id, int px, int py, int w, int h) {
        bid = id;
        posX = px;
        posY = py;
        width = w;
        height = h;
      }
      UIButton(int id, int px, int py, int w, int h, std::string txt) {
        bid = id;
        posX = px;
        posY = py;
        width = w;
        height = h;
        text = txt;
      }
      UIButton(int id, int px, int py, int w, int h, std::string txt, Font* fontPtr) {
        bid = id;
        posX = px;
        posY = py;
        width = w;
        height = h;
        text = txt;
        fontRef = fontPtr;
      }
      bool relativeToCenter = true;
      int bid = 0;
      int posX = 0;
      int posY = 0;
      int width = 100;
      int height = 40;
      int fontSize = 18;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = GRAY;
      Color txtColor = BLACK;
      std::string text = "";
      Font* fontRef = NULL;
      bool isHovered = false;
      void update(Vector2 mousePos);
      void render();
      void render(Vector2 center);
    private:
  };
  class EventLoop {
    public:
      // global states
      int screenW = 0;
      int screenH = 0;
      Vector2 screenCenter = { 0.0, 0.0 };
      int fps = 0;
      double elapsed = 0.0;
      // data objects
      DbI::DbInterface dbInterface;
      std::vector<DbI::EavItem> eavCategories;
      // ui objects
      Font font;
      std::vector<UIButton> btns;
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