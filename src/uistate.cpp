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
  // clickActionAvailable = true;
  // uiEvent = UI_NONE;
  mouseState = MOUSE_NONE;
  mousePos = GetMousePosition();
  mouseDelta = GetMouseDelta();
  timeDelta = GetFrameTime();
  hoverId = 0;
  invisHover = false;
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_HOLD;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_UP;
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) rMouseState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) rMouseState = MOUSE_UP;
  // prevent mouse events when holding something
  // if (mouseState == MOUSE_HOLD && holdIds.size() > 0) {
  //   clickId = -1;
  //   hoverId = -1;
  // }
}

void UIState::postUpdate() {
  // release held objects
  if (mouseState == MOUSE_UP) clickId = 0;
  if (mouseState == MOUSE_UP && holdIds.size() > 0) {
    holdIds.clear();
  }
  // update mouse state
  if (clickId != 0 || (!invisHover && hoverId != 0)) {
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  } else {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
  }
}

bool UIState::uiIsHovering(int id) {
  return id == hoverId;
}

bool UIState::uiIsClicking(int id) {
  return id == clickId;
}

bool UIState::uiIsHolding(int id) {
  bool o = false;
  for (int i : holdIds) {
    if (i == id) o = true;
  }
  return o;
}

void UIState::uiStartHolding(int id) {
  holdIds.push_back(id);
}

void UIState::uiStartHolding(std::vector<int> ids) {
  for (int i : ids) {
    holdIds.push_back(i);
  }
}