#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIEvent UIButtonBase::update(Vector2 ctr, Vector2 mPos) {
  if (!relativeToCenter) ctr = { 0.0f, 0.0f };
  float mdx = mPos.x - originalMouseLock.x;
  float mdy = mPos.y - originalMouseLock.y;
  // offset center when held by mouse
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && holding) {
    ctr.x += mdx;
    ctr.y += mdy;
  }
  // calculate absolute position based on center
  int absX = ctr.x + posSize.x - posSize.width / 2;
  int absY = ctr.y + posSize.y - posSize.height / 2;
  _absPos = { (float)absX, (float)absY, posSize.width, posSize.height };

  // calculate if btn is being hovered
  UIEvent event = NO_EVENT;
  Rectangle bounds = { (float)absX, (float)absY, posSize.width, posSize.height };
  Color clr = btnColor;

  if (CheckCollisionPointRec(mPos, bounds)) {
    event = BTN_HOVER;
    clr = btnHoverColor;
  }
  if (event == BTN_HOVER && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    event = BTN_HOLD;
    clr = btnDownColor;
    originalMouseLock.x = mPos.x;
    originalMouseLock.y = mPos.y;
    holding = true;
  }
  if (holding && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    event = BTN_HOLD;
    clr = btnDownColor;
  }
  if (holding && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    event = BTN_CLICK;
    clr = btnDownColor;
    posSize.x += mdx;
    posSize.y += mdy;
    originalMouseLock = { 0.0f, 0.0f };
    holding = false;
  }

  Vector2 txtDim = MeasureTextEx(font, displayTxt.c_str(), fontSize, 0.0);
  _txtPos = {ctr.x + posSize.x - txtDim.x / 2, ctr.y + posSize.y - txtDim.y / 2};
  
  return event;
}

void UIButtonBase::render() {
  // draw background
  DrawRectangle(_absPos.x, _absPos.y, _absPos.width, _absPos.height, _activeColor);
  // draw text
  if (displayTxt != "") {
    DrawTextEx(font, displayTxt.c_str(), _txtPos, fontSize, 0.0, txtColor);
  }
}