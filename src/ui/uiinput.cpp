#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

UIInput::UIInput(UIState* globalState) {
  state = globalState;
  if (state != NULL) id = state->getNewId();
  _mask = LoadRenderTexture(posSize.width, posSize.height);
}

UIInput::UIInput(UIState* globalState, Rectangle bounds) {
  state = globalState;
  if (state != NULL) id = state->getNewId();
  posSize = bounds;
  _mask = LoadRenderTexture(posSize.width, posSize.height);
}

void UIInput::clear() {
  input.clear();
  _txtPos.x = 5.0f;
}

bool UIInput::update() {
  if (state == NULL) return false;
  UIEvent evt = state->componentUpdate(id, &posSize);
  if (evt == UI_CLICK) isActive = true;
  // resize mask
  if (state->screenUpdate) {
    _mask = LoadRenderTexture(posSize.width, posSize.height);
  }
  // handle keyboard input
  if (isActive) {
    // capture key inputs
    float dt = GetFrameTime();
    int key = GetCharPressed();
    while (key != 0) {
      // note: handles unicode characters only
      if (key >= 32 && key <= 125) {
        input += (char)key;
        _updateTextPos();
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_ENTER)) isActive = false;
    if (IsKeyPressed(KEY_BACKSPACE) && !input.empty()) {
      input.pop_back();
      _bkspCooldown = 0.5f;
    } else if (IsKeyDown(KEY_BACKSPACE) && input.size() > 0) {
      // check cooldown
      if (_bkspCooldown > 0.0f) {
        _bkspCooldown -= dt;
      }
      if (_bkspCooldown <= 0.0f) {
        input.pop_back();
        _updateTextPos();
        _bkspCooldown = 0.06f;
      }
    }

    if (state->uiIsClickedOutside(id)) {
      isActive = false;
    }

    // update blinker
    _blinkTimer += dt;
    if (_blinkTimer > 0.8f) {
      _blinkState = _blinkState == 1 ? 0 : 1;
      _blinkTimer = 0.0f;
    }
  } else if (_blinkTimer != 0.5f) {
    // reset blinker
    _blinkState = 0;
    _blinkTimer = 0.5f;
  }
  return evt == UI_CLICK;
}

void UIInput::render() {
  if (state == NULL) return;
  // set color
  Color activeColor = boxColor;
  if (isActive) activeColor = boxActiveColor;
  else if (state->uiIsHovering(id)) activeColor = boxHoverColor;
  // draw masked texture
  BeginTextureMode(_mask);
    // draw input bg
    ClearBackground(activeColor);
    DrawRectangle(0, 0, posSize.width, posSize.height, shadowColor);
    DrawRectangle(0, 0, (posSize.width - 4.0f), (posSize.height - 4.0f), activeColor);
    // draw text
    if (input != "") {
      DrawTextEx(state->font, input.c_str(), _txtPos, fontSize, 0.0, txtColor);
    } else if (!isActive && placeholder != "") {
      DrawTextEx(state->font, placeholder.c_str(), _txtPos, fontSize, 0.0, placeholderColor);
    }
  EndTextureMode();
  DrawTextureRec(
    _mask.texture,
    Rectangle { 0, 0, posSize.width, -1 * posSize.height },
    Vector2{ posSize.x, posSize.y },
    WHITE
  );
  // draw blinker
  if (_blinkState) {
    Vector2 textSize = MeasureTextEx(state->font, input.c_str(), fontSize, 0.0f);
    float x = posSize.x + textSize.x + 5.0f;
    float xLimit = posSize.x + posSize.width - 4.0f;
    if (x > xLimit) x = xLimit;
    DrawLineEx(
      Vector2{x, posSize.y + 2.0f },
      Vector2{x, posSize.y + posSize.height - 4.0f},
      2.0f,
      Color { 40, 20, 20, 255 }
    );
  }
  // draw border
  DrawRectangleLinesEx(posSize, 1.0f, borderColor);
}

void UIInput::cleanup() {
  UnloadRenderTexture(_mask);
}

void UIInput::_updateTextPos() {
  Vector2 txtBounds = MeasureTextEx(state->font, input.c_str(), fontSize, 0.0f);
  float dw = txtBounds.x - posSize.width + 12.0f;
  if (dw > 0.0f) _txtPos.x = 5.0f - dw;
  else _txtPos.x = 5.0f;
}
