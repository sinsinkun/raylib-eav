#include <vector>
#include <string>
#include <raylib.h>
#include "dbi.hpp"

namespace App {
  // generic UI utils
  enum UIEvent { UI_NONE, UI_HOVER_INVIS, UI_HOVER, UI_HOLD, UI_CLICK };
  enum MouseState { MOUSE_NONE, MOUSE_OVER, MOUSE_DOWN, MOUSE_HOLD, MOUSE_UP };
  // generic UI classes
  class UIInput {
    public:
      UIInput();
      UIInput(Rectangle bounds);
      int id = 0;
      Rectangle posSize = { 0.0f, 0.0f, 200.0f, 30.0f };
      std::string placeholder = "Type Input Here";
      std::string input = "";
      int maxInputSize = 255;
      Font font = GetFontDefault();
      float fontSize = 18.0f;
      bool isActive = false;
      Color boxColor = Color { 140, 140, 140, 255 };
      Color boxHoverColor = Color { 160, 160, 160, 255 };
      Color boxActiveColor = Color { 210, 200, 240, 255 };
      Color shadowColor = Color { 0, 0, 0, 100 };
      Color borderColor = Color { 0, 0, 0, 180 };
      Color txtColor = BLACK;
      Color placeholderColor = DARKGRAY;
      UIEvent update(MouseState mouseState);
      UIEvent update(MouseState mouseState, bool isNotHovering);
      void render();
      void cleanup();
    private:
      RenderTexture2D _mask;
      Color _activeColor = boxColor;
      Vector2 _txtPos { 5.0f, 5.0f };
      float _bkspCooldown = 0.0f;
      int _blinkState = 0;
      float _blinkTimer = 0.0f;
      void _updateTextPos();
  };
  class UIButtonBase {
    public:
      int id = 0;
      std::string displayTxt = "";
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      Font font = GetFontDefault();
      float fontSize = 18.0f;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = Color { 220, 220, 220, 255 };
      Color btnDownColor = Color { 230, 230, 240, 255 };
      Color txtColor = BLACK;
      UIEvent update(MouseState mouseState);
      void render();
    private:
      Vector2 _txtPos { 0.0f, 0.0f };
      Color _activeColor = btnColor;
  };
  class UIBox {
    public:
      int id = 0;
      std::string title = "";
      std::vector<std::string> body;
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      Font font = GetFontDefault();
      float titleFontSize = 18.0f;
      float bodyFontSize = 16.0f;
      bool renderShadow = true;
      bool renderBorder = true;
      Color boxColor = LIGHTGRAY;
      Color boxHoverColor = Color { 220, 220, 220, 255 };
      Color shadowColor = Color { 0, 0, 0, 50 };
      Color borderColor = Color { 0, 0, 0, 140 };
      Color txtColor = BLACK;
      UIEvent update(MouseState mouseState);
      UIEvent update(MouseState mouseState, Vector2 deltaPos);
      void render();
    private:
      Color _activeColor = boxColor;
  };
  // specific use
  class DialogBox {
    public:
      DialogBox() {
        box.boxColor = LIGHTGRAY;
        box.boxHoverColor = LIGHTGRAY;
      };
      DialogBox(Rectangle bounds, std::string titleIn, Font ft);
      bool isVisible = true;
      UIBox box;
      UIInput input;
      UIButtonBase btn;
      UIEvent update(Vector2 mousePos, MouseState mouseState);
      UIEvent update(Vector2 mousePos, MouseState mouseState, Vector2 mouseDelta, void** grabbedObjPtr);
      void render();
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
  class EavEntity {
    public:
      EavEntity(DbI::EavItem item, Rectangle posSizeIn, Font fontIn, DbI::DbInterface* dbi);
      int id = 0;
      std::string name = "";
      std::vector<DbI::EavItem> values;
      UIBox box;
      void fillBody();
      UIEvent update(Vector2 mPos, MouseState mState);
      UIEvent update(Vector2 mousePos, MouseState mouseState, Vector2 mDelta, void** gObjPtr);
      void render();
  };
  class EventLoop {
    public:
      // global states
      int screenW = 0;
      int screenH = 0;
      Vector2 screenCenter = { 0.0f, 0.0f };
      Vector2 mousePos = { 0.0f, 0.0f };
      void* grabbedObject = NULL;
      int fps = 0;
      double elapsed = 0.0;
      Font font;
      Color bgColor = Color { 35, 35, 40, 255 };
      // data objects
      DbI::DbInterface dbInterface;
      std::vector<EavBlueprint> categories;
      std::vector<EavEntity> entities;
      DialogBox dialog;
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