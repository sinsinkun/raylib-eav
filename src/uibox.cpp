#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIEvent UIBox::update(MouseState mState) {
  // calculate if box is being hovered
  UIEvent event = UI_NONE;
  switch (mState) {
    case MOUSE_OVER:
      event = UI_HOVER_INVIS;
      _activeColor = boxHoverColor;
      break;
    case MOUSE_DOWN:
    case MOUSE_HOLD:
      event = UI_HOLD;
      _activeColor = boxHoverColor;
      break;
    case MOUSE_UP:
      event = UI_CLICK;
      _activeColor = boxHoverColor;
      break;
    case MOUSE_NONE:
    default:
      _activeColor = boxColor;
      break;
  }

  return event;
}

void UIBox::render() {
  if (renderShadow) {
    DrawRectangle(posSize.x - 5, posSize.y - 5, posSize.width + 10, posSize.height + 10, shadowColor);
  }
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, _activeColor);
  // draw text
  if (title != "") {
    Vector2 titlePos = { posSize.x + 5.0f, posSize.y + 5.0f };
    DrawTextEx(font, title.c_str(), titlePos, titleFontSize, 0.0, txtColor);
  }
  if (renderBorder) {
    DrawRectangleLinesEx(posSize, 2.0f, borderColor);
  }
}