#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

bool UIButton::update() {
  if (state == NULL) return false;
  MouseState mState = MOUSE_NONE;
  UIEvent event = UI_NONE;
  // calculate if btn is being hovered
  if (CheckCollisionPointRec(state->mousePos, posSize) && state->uiEvent == UI_NONE) {
    if (state->mouseState == MOUSE_NONE) mState = MOUSE_OVER;
    else mState = state->mouseState;
  }
  
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

  // text position updates
  Vector2 txtDim = MeasureTextEx(state->font, text.c_str(), fontSize, 0.0);
  float txtX = posSize.x + (posSize.width - txtDim.x) / 2.0f;
  float txtY = posSize.y + (posSize.height - txtDim.y) / 2.0f;
  _txtPos = {txtX, txtY};
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
  if (isClicking) state->clickActionAvailable = false;
  return isClicking;
}

void UIButton::render() {
  if (state == NULL) return;
  // draw background
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, _activeColor);
  // draw text
  if (text != "") {
    DrawTextEx(state->font, text.c_str(), _txtPos, fontSize, 0.0, txtColor);
  }
}
