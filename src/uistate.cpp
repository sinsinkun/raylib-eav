#include <iostream>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

int UIState::getNewId() {
  int thisId = uiId;
  uiId++;
  return thisId;
}

void UIState::update() {
  screen.width = GetScreenWidth();
  screen.height = GetScreenHeight();
  screen.x = screen.width / 2.0f;
  screen.y = screen.height / 2.0f;
  mouseState = MOUSE_NONE;
  mousePos = GetMousePosition();
  mouseDelta = GetMouseDelta();
  timeDelta = GetFrameTime();
  hoverId = 0;
  invisHover = false;
  clickFrame = false;
  rClickFrame = false;
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_HOLD;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) mouseState = MOUSE_UP;
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) rMouseState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) rMouseState = MOUSE_UP;
  // prevent mouse events when holding something
  if (mouseState == MOUSE_HOLD && holdIds.size() > 0) {
    clickId = -1;
    hoverId = -1;
  }
}

void UIState::postUpdate() {
  // release held objects
  if (mouseState == MOUSE_UP) clickId = 0;
  if (rMouseState == MOUSE_UP) rClickId = 0;
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

UIEvent UIState::componentUpdate(int id, Rectangle* posSize) {
  return componentUpdate(id, posSize, false, false);
}

UIEvent UIState::componentUpdate(int id, Rectangle* posSize, bool clickOnDown, bool hideHover) {
  UIEvent evt = UI_NONE;
  // calculate if component is being hovered
  if (CheckCollisionPointRec(mousePos, *posSize) && hoverId == 0) {
    hoverId = id;
    evt = UI_HOVER;
    invisHover = hideHover;
    if (mouseState == MOUSE_NONE) mouseState = MOUSE_OVER;
    MouseState clickState = clickOnDown ? MOUSE_DOWN : MOUSE_UP;
    if (mouseState == clickState && clickId == 0) {
      clickId = id;
      clickFrame = true;
      evt = UI_CLICK;
    }
    if (rMouseState == MOUSE_DOWN && rClickId == 0) {
      rClickId = id;
      rClickFrame = true;
      evt = evt == UI_CLICK ? UI_BOTH_CLICK : UI_R_CLICK;
    }
  }

  // handle drag event
  if (mouseState == MOUSE_HOLD && uiIsHolding(id)) {
    posSize->x += mouseDelta.x;
    posSize->y += mouseDelta.y;
    if (evt < UI_HOLD) evt = UI_HOLD;
  }
  // state updates
  return evt;
}

bool UIState::uiIsClicked(int id) {
  return clickId == id && clickFrame;
}

bool UIState::uiIsClickedOutside(int id) {
  return clickId != id && mouseState == MOUSE_DOWN;
};

bool UIState::uiIsClickedOutside(Rectangle* bounds) {
  bool inside = CheckCollisionPointRec(mousePos, *bounds);
  return !inside && mouseState == MOUSE_DOWN;
};

bool UIState::uiIsRClicked(int id) {
  return rClickId == id && rClickFrame;
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