#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIEvent UIInput::update(MouseState mState) {
  UIEvent event = UI_NONE;
  switch (mState) {
    case MOUSE_UP:
    case MOUSE_OVER:
      event = UI_HOVER;
      if (isActive) _activeColor = boxActiveColor;
      else _activeColor = boxHoverColor;
      break;
    case MOUSE_HOLD:
      event = UI_HOLD;
      _activeColor = boxActiveColor;
      break;
    case MOUSE_DOWN:
      event = UI_CLICK;
      _activeColor = boxActiveColor;
      isActive = !isActive;
      break;
    case MOUSE_NONE:
    default:
      if (isActive) _activeColor = boxActiveColor;
      else _activeColor = boxColor;
      break;
  }

  // capture key inputs
  if (isActive) {
    int key = GetCharPressed();
    if (key >= 32 && key <= 125) {
      input += (char)key;
    }
    if (IsKeyPressed(KEY_ENTER)) isActive = false;
    if (IsKeyDown(KEY_BACKSPACE) && input.size() > 0) {
      // check cooldown
      if (bkspCooldown > 0.0f) {
        bkspCooldown -= GetFrameTime();
      }
      if (bkspCooldown <= 0.0f) {
        input.pop_back();
        bkspCooldown = 0.06f;
      }
    }
  }
  return event;
}

UIEvent UIInput::update(MouseState mState, bool noHover) {
  if (noHover && mState == MOUSE_DOWN) {
    isActive = false;
    _activeColor = boxColor;
  }
  return UI_NONE;
}

void UIInput::render() {
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, _activeColor);
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, shadowColor);
  DrawRectangle(posSize.x, posSize.y, (posSize.width - 4.0f), (posSize.height - 4.0f), _activeColor);
  // draw text
  Vector2 textPos = {posSize.x + 5.0f, posSize.y + 5.0f };
  if (input != "") {
    DrawTextEx(font, input.c_str(), textPos, fontSize, 0.0, txtColor);
  } else if (placeholder != "") {
    DrawTextEx(font, placeholder.c_str(), textPos, fontSize, 0.0, placeholderColor);
  }
  DrawRectangleLinesEx(posSize, 1.0f, borderColor);
}