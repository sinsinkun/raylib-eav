#pragma once

#include <vector>
#include <string>
#include <raylib.h>
#include "dbi.hpp"

namespace App {
  enum UIEvent { UI_NONE, UI_HOVER_INVIS, UI_HOVER, UI_HOLD, UI_CLICK };
  enum MouseState { MOUSE_NONE, MOUSE_OVER, MOUSE_DOWN, MOUSE_HOLD, MOUSE_UP };
  class UIState {
    public:
      Vector2 screenCenter = { 0.0f, 0.0f };
      Font font = GetFontDefault();
      float timeDelta = 0.0f;
      Vector2 mousePos = { 0.0f, 0.0f };
      Vector2 mouseDelta = { 0.0f, 0.0f };
      MouseState mouseState = MOUSE_NONE;
      MouseState rMouseState = MOUSE_NONE;
      bool invisHover = false;
      bool clickFrame = false;
      bool rClickFrame = false;
      int hoverId = 0;
      int clickId = 0;
      int rClickId = 0;
      std::vector<int> holdIds;
      int getNewId();
      void update();
      void postUpdate();
      bool uiIsClicked(int id);
      bool uiIsRClicked(int id);
      bool uiIsHovering(int id);
      bool uiIsClicking(int id);
      bool uiIsHolding(int id);
      void uiStartHolding(int id);
      void uiStartHolding(std::vector<int> id);
    private:
      int _uiId = 1;
  };
  class UIInput {
    public:
      UIInput(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        _mask = LoadRenderTexture(posSize.width, posSize.height);
      }
      UIInput(UIState* globalState, Rectangle bounds) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        posSize = bounds;
        _mask = LoadRenderTexture(posSize.width, posSize.height);
      }
      UIState* state = NULL;
      int id = 0;
      Rectangle posSize = { 0.0f, 0.0f, 200.0f, 30.0f };
      std::string placeholder = "Type Input Here";
      std::string input = "";
      int maxInputSize = 255;
      float fontSize = 18.0f;
      bool isActive = false;
      bool disabled = false;
      Color boxColor = Color { 140, 140, 140, 255 };
      Color boxHoverColor = Color { 160, 160, 160, 255 };
      Color boxActiveColor = Color { 210, 200, 240, 255 };
      Color shadowColor = Color { 0, 0, 0, 100 };
      Color borderColor = Color { 0, 0, 0, 180 };
      Color txtColor = BLACK;
      Color placeholderColor = DARKGRAY;
      void clear();
      bool update();
      void render();
      void cleanup();
    private:
      RenderTexture2D _mask;
      // Color _activeColor = boxColor;
      Vector2 _txtPos { 5.0f, 5.0f };
      float _bkspCooldown = 0.0f;
      int _blinkState = 0;
      float _blinkTimer = 0.0f;
      void _updateTextPos();
  };
  class UIButton {
    public:
      UIButton(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
      }
      UIButton(UIState* globalState, Rectangle posSizeIn, std::string textIn) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        text = textIn;
        posSize = posSizeIn;
        Vector2 txtDim = MeasureTextEx(state->font, text.c_str(), fontSize, 0.0);
        if (txtDim.x > posSize.width) posSize.width = txtDim.x + 10.0f;
        if (txtDim.y > posSize.height) posSize.height = txtDim.y + 10.0f;
      }
      UIState* state = NULL;
      int id = 0;
      std::string text = "";
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      float fontSize = 18.0f;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = Color { 220, 220, 220, 255 };
      Color btnDownColor = Color { 230, 230, 240, 255 };
      Color txtColor = BLACK;
      bool update();
      void render();
  };
  class UIBox {
    public:
      UIBox(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
      }
      UIState* state = NULL;
      int id = 0;
      bool hideHover = false;
      std::string title = "";
      std::vector<std::string> body;
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      float titleFontSize = 18.0f;
      float bodyFontSize = 16.0f;
      bool renderShadow = true;
      bool renderBorder = true;
      Color boxColor = LIGHTGRAY;
      Color boxHoverColor = Color { 220, 220, 220, 255 };
      Color shadowColor = Color { 0, 0, 0, 50 };
      Color borderColor = Color { 0, 0, 0, 140 };
      Color txtColor = BLACK;
      bool update();
      void render();
  };
}