#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIInput::UIInput() {
  _mask = LoadRenderTexture(posSize.width, posSize.height);
}

UIInput::UIInput(Rectangle bounds) {
  posSize = bounds;
  _mask = LoadRenderTexture(posSize.width, posSize.height);
}

UIEvent UIInput::update(MouseState mState) {
  return update(mState, false);
}

UIEvent UIInput::update(MouseState mState, bool noHover) {
  UIEvent event = UI_NONE;
  switch (mState) {
    case MOUSE_UP:
    case MOUSE_OVER:
      event = UI_HOVER;
      if (isActive) _activeColor = boxActiveColor;
      else if (!noHover) _activeColor = boxHoverColor;
      break;
    case MOUSE_HOLD:
      event = UI_HOLD;
      if (!noHover) _activeColor = boxActiveColor;
      break;
    case MOUSE_DOWN:
      event = UI_CLICK;
      isActive = !noHover;
      if (!noHover && isActive) _activeColor = boxActiveColor;
      else _activeColor = boxColor;
      break;
    case MOUSE_NONE:
    default:
      if (isActive) _activeColor = boxActiveColor;
      else _activeColor = boxColor;
      break;
  }

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
    
    if (_blinkState == 1) {
      std::cout << "blink" << std::endl;
    }
    // update blinker
    _blinkTimer += dt;
    if (_blinkTimer > 0.5f) {
      _blinkState = 1 ? 0 : 1;
      _blinkTimer = 0.0f;
    }
  } else if (_blinkTimer != 0.0f) {
    // reset blinker
    _blinkState = 0;
    _blinkTimer = 0.0f;
  }

  // update
  if (noHover) return UI_NONE;
  return event;
}

void UIInput::render() {
  BeginTextureMode(_mask);
    // draw input bg
    ClearBackground(_activeColor);
    DrawRectangle(0, 0, posSize.width, posSize.height, shadowColor);
    DrawRectangle(0, 0, (posSize.width - 4.0f), (posSize.height - 4.0f), _activeColor);
    // draw text
    if (input != "") {
      DrawTextEx(font, input.c_str(), _txtPos, fontSize, 0.0, txtColor);
    } else if (!isActive && placeholder != "") {
      DrawTextEx(font, placeholder.c_str(), _txtPos, fontSize, 0.0, placeholderColor);
    }
  EndTextureMode();
  DrawTextureRec(
    _mask.texture,
    Rectangle { 0, 0, posSize.width, -1 * posSize.height },
    Vector2{ posSize.x, posSize.y},
    WHITE
  );
  // draw border
    DrawRectangleLinesEx(posSize, 1.0f, borderColor);
}

void UIInput::cleanup() {
  UnloadRenderTexture(_mask);
}

void UIInput::_updateTextPos() {
  Vector2 txtBounds = MeasureTextEx(font, input.c_str(), fontSize, 0.0f);
  float dw = txtBounds.x - posSize.width + 12.0f;
  if (dw > 0.0f) _txtPos.x = 5.0f - dw;
  else _txtPos.x = 5.0f;
}
