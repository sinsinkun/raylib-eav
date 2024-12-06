#include <iostream>
#include <raylib.h>
#include "app.hpp"

using namespace App;

int UIState::getNewId() {
  int thisId = _uiId;
  _uiId++;
  return thisId;
}

void UIState::update() {
  clickActionAvailable = true;
  uiEvent = UI_NONE;
  mouseState = MOUSE_NONE;
  mousePos = GetMousePosition();
  mouseDelta = GetMouseDelta();
  timeDelta = GetFrameTime();
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_HOLD;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_UP;
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) rMouseState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) rMouseState = MOUSE_UP;
  // release active drag component
  if (mouseState == MOUSE_HOLD && activeDragId != -1) clickActionAvailable = false;
  if (mouseState == MOUSE_UP && activeDragId != -1) activeDragId = -1;
}
