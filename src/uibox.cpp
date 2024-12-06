#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

bool UIBox::update() {
  if (state == NULL) return false;
  // calculate if box is being hovered
  bool clicked = false;
  if (CheckCollisionPointRec(state->mousePos, posSize) && state->hoverId == 0) {
    state->hoverId = id;
    if (hideHover) state->invisHover = true;
    if (state->mouseState == MOUSE_NONE) state->mouseState = MOUSE_OVER;
    if (state->mouseState == MOUSE_DOWN && state->clickId == 0) {
      state->clickId = id;
      clicked = true;
    }
  }

  // handle drag event
  if (state->mouseState == MOUSE_HOLD && state->uiIsHolding(id)) {
    posSize.x += state->mouseDelta.x;
    posSize.y += state->mouseDelta.y;
  }
  // state updates
  return clicked;
}

void UIBox::render() {
  if (state == NULL) return;
  if (renderShadow) {
    DrawRectangle(posSize.x - 2, posSize.y - 2, posSize.width + 6, posSize.height + 7, shadowColor);
  }
  Color activeColor = boxColor;
  if (state->hoverId == id) activeColor = boxHoverColor;
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, activeColor);
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