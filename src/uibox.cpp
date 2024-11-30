#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIEvent UIBox::update(MouseState mState) {
  Vector2 d = {0.0f, 0.0f};
  return update(mState, d);
}

UIEvent UIBox::update(MouseState mState, Vector2 dPos) {
  // calculate if box is being hovered
  UIEvent event = UI_NONE;
  switch (mState) {
    case MOUSE_UP:
    case MOUSE_OVER:
      event = UI_HOVER_INVIS;
      _activeColor = boxHoverColor;
      break;
    case MOUSE_HOLD:
      event = UI_HOLD;
      _activeColor = boxHoverColor;
      break;
    case MOUSE_DOWN:
      event = UI_CLICK;
      _activeColor = boxHoverColor;
      break;
    case MOUSE_NONE:
    default:
      _activeColor = boxColor;
      break;
  }
  posSize.x += dPos.x;
  posSize.y += dPos.y;
  return event;
}

void UIBox::render() {
  if (renderShadow) {
    DrawRectangle(posSize.x - 2, posSize.y - 2, posSize.width + 5, posSize.height + 8, shadowColor);
  }
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, _activeColor);
  // draw text
  if (title != "") {
    Vector2 titlePos = { posSize.x + 5.0f, posSize.y + 5.0f };
    DrawTextEx(font, title.c_str(), titlePos, titleFontSize, 0.0, txtColor);
  }
  if (body.size() > 0) {
    for (int i=0; i < body.size(); i++) {
      Vector2 pos = { posSize.x + 5.0f, posSize.y + 30 + i*(bodyFontSize + 2.0f) };
      DrawTextEx(font, body[i].c_str(), pos, bodyFontSize, 0.0f, txtColor);
    }
  }
  if (renderBorder) {
    DrawRectangleLinesEx(posSize, 1.0f, borderColor);
  }
}