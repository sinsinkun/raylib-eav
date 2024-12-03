#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

bool UIInput::update() {
  if (state == NULL) return false;
  MouseState mState = MOUSE_NONE;
  UIEvent event = UI_NONE;
  bool isHovering = false;
  // calculate if input is being hovered
  if (CheckCollisionPointRec(state->mousePos, posSize) && state->uiEvent == UI_NONE) {
    if (state->mouseState == MOUSE_NONE) mState = MOUSE_OVER;
    else mState = state->mouseState;
    isHovering = true;
  }

  switch (mState) {
    case MOUSE_UP:
    case MOUSE_OVER:
      event = UI_HOVER;
      if (isActive) _activeColor = boxActiveColor;
      else if (isHovering) _activeColor = boxHoverColor;
      break;
    case MOUSE_HOLD:
      event = UI_HOLD;
      if (isHovering) _activeColor = boxActiveColor;
      break;
    case MOUSE_DOWN:
      event = UI_CLICK;
      isActive = isHovering;
      if (isHovering) _activeColor = boxActiveColor;
      else _activeColor = boxColor;
      break;
    case MOUSE_NONE:
    default:
      if (isActive) _activeColor = boxActiveColor;
      else _activeColor = boxColor;
      break;
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
  if (dragId != 0 && event == UI_CLICK && state->activeDragId == -1) {
    state->activeDragId = dragId;
  } else if (dragId != 0 && state->mouseState == MOUSE_HOLD && dragId == state->activeDragId) {
    posSize.x += state->mouseDelta.x;
    posSize.y += state->mouseDelta.y;
  }
  // state updates
  if (event > state->uiEvent) state->uiEvent = event;
  bool isClicking = event == UI_CLICK && state->clickActionAvailable;
  if (isClicking) {
    if (isActive && !isHovering) isActive = false;
    state->clickActionAvailable = false;
  }
  return isClicking;
}

void UIInput::render() {
  if (state == NULL) return;
  // draw masked texture
  BeginTextureMode(_mask);
    // draw input bg
    ClearBackground(_activeColor);
    DrawRectangle(0, 0, posSize.width, posSize.height, shadowColor);
    DrawRectangle(0, 0, (posSize.width - 4.0f), (posSize.height - 4.0f), _activeColor);
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
