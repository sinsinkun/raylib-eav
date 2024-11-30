#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIEvent UIButtonBase::update(MouseState mState) {
  // calculate if btn is being hovered
  UIEvent event = UI_NONE;
  switch (mState) {
    case MOUSE_OVER:
      event = UI_HOVER;
      _activeColor = btnHoverColor;
      break;
    case MOUSE_DOWN:
    case MOUSE_HOLD:
      event = UI_HOLD;
      _activeColor = btnDownColor;
      break;
    case MOUSE_UP:
      event = UI_CLICK;
      _activeColor = btnDownColor;
      break;
    case MOUSE_NONE:
    default:
      _activeColor = btnColor;
      break;
  }

  Vector2 txtDim = MeasureTextEx(font, displayTxt.c_str(), fontSize, 0.0);
  float txtX = posSize.x + (posSize.width - txtDim.x) / 2.0f;
  float txtY = posSize.y + (posSize.height - txtDim.y) / 2.0f;
  _txtPos = {txtX, txtY};
  
  return event;
}

void UIButtonBase::render() {
  // draw background
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, _activeColor);
  // draw text
  if (displayTxt != "") {
    DrawTextEx(font, displayTxt.c_str(), _txtPos, fontSize, 0.0, txtColor);
  }
}