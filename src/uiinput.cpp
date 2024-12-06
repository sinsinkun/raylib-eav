#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

bool UIInput::update() {
  if (state == NULL) return false;
  bool isHovering = false;
  bool clicked = false;
  if (CheckCollisionPointRec(state->mousePos, posSize) && state->hoverId == 0) {
    isHovering = true;
    state->hoverId = id;
    if (state->mouseState == MOUSE_NONE) state->mouseState = MOUSE_OVER;
    if (state->mouseState == MOUSE_DOWN && state->clickId == 0) {
      state->clickId = id;
      state->clickFrame = true;
      clicked = true;
      if (!disabled) isActive = true;
    }
    if (state->rMouseState == MOUSE_DOWN && state->rClickId == 0) {
      state->rClickId = id;
      state->rClickFrame = true;
    }
  }
  // handle keyboard input
  if (isActive) {
    // capture key inputs
    float dt = GetFrameTime();
    int key = GetCharPressed();
    if (key >= 32 && key <= 125 && input.size() < maxInputSize) {
      input += (char)key;
      _updateTextPos();
    }
    if (IsKeyPressed(KEY_ENTER)) isActive = false;
    if (IsKeyDown(KEY_BACKSPACE) && input.size() > 0) {
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

    if (state->mouseState == MOUSE_DOWN && !isHovering) {
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

  // handle drag event
  if (state->mouseState == MOUSE_HOLD && state->uiIsHolding(id)) {
    posSize.x += state->mouseDelta.x;
    posSize.y += state->mouseDelta.y;
  }
  return clicked;
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
    DrawLineEx(
      Vector2{posSize.x + posSize.width - 4.0f, posSize.y + 2.0f },
      Vector2{posSize.x + posSize.width - 4.0f, posSize.y + posSize.height - 4.0f},
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
