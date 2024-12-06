#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

bool UIButton::update() {
  if (state == NULL) return false;
  // calculate if btn is being hovered
  bool clicked = false;
  if (CheckCollisionPointRec(state->mousePos, posSize) && state->hoverId == 0) {
    state->hoverId = id;
    if (state->mouseState == MOUSE_NONE) state->mouseState = MOUSE_OVER;
    if (state->mouseState == MOUSE_DOWN && state->clickId == 0) {
      state->clickId = id;
      state->clickFrame = true;
      clicked = true;
    }
    if (state->rMouseState == MOUSE_DOWN && state->rClickId == 0) {
      state->rClickId = id;
      state->rClickFrame = true;
    }
  }

  // handle drag event
  if (state->mouseState == MOUSE_HOLD && state->uiIsHolding(id)) {
    posSize.x += state->mouseDelta.x;
    posSize.y += state->mouseDelta.y;
  }
  return clicked;
}

void UIButton::render() {
  if (state == NULL) return;
  // draw background
  Color activeColor = btnColor;
  if (state->uiIsClicking(id)) activeColor = btnDownColor;
  else if (state->uiIsHovering(id)) activeColor = btnHoverColor;
  DrawRectangle(posSize.x, posSize.y, posSize.width, posSize.height, activeColor);
  // draw text
  if (text != "") {
    Vector2 txtDim = MeasureTextEx(state->font, text.c_str(), fontSize, 0.0);
    float txtX = posSize.x + (posSize.width - txtDim.x) / 2.0f;
    float txtY = posSize.y + (posSize.height - txtDim.y) / 2.0f;
    DrawTextEx(state->font, text.c_str(), Vector2{ txtX, txtY }, fontSize, 0.0, txtColor);
  }
}
