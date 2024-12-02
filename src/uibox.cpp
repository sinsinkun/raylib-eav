#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

bool UIBox::update() {
  if (state == NULL) return false;
  MouseState mState = MOUSE_NONE;
  UIEvent event = UI_NONE;
  // calculate if box is being hovered
  if (CheckCollisionPointRec(state->mousePos, posSize) && state->uiEvent == UI_NONE) {
    if (state->mouseState == MOUSE_NONE) mState = MOUSE_OVER;
    else mState = state->mouseState;
  }

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

void UIBox::render() {
  if (state == NULL) return;
  if (renderShadow) {
    DrawRectangle(posSize.x - 2, posSize.y - 2, posSize.width + 6, posSize.height + 7, shadowColor);
  }
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, _activeColor);
  // draw text
  if (title != "") {
    Vector2 titlePos = { posSize.x + 5.0f, posSize.y + 5.0f };
    DrawTextEx(state->font, title.c_str(), titlePos, titleFontSize, 0.0, txtColor);
  }
  if (body.size() > 0) {
    for (int i=0; i < body.size(); i++) {
      Vector2 pos = { posSize.x + 5.0f, posSize.y + 30 + i*(bodyFontSize + 2.0f) };
      DrawTextEx(state->font, body[i].c_str(), pos, bodyFontSize, 0.0f, txtColor);
    }
  }
  if (renderBorder) {
    DrawRectangleLinesEx(posSize, 1.0f, borderColor);
  }
}