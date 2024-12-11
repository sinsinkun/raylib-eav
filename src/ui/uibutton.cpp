#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

bool UIButton::update() {
  if (state == NULL) return false;
  UIEvent evt = state->componentUpdate(id, &posSize);
  return evt == UI_CLICK;
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
  // draw border
  if (renderBorder) {
    DrawRectangleLinesEx(posSize, 1.0f, borderColor);
  }
}
