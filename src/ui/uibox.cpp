#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

bool UIBox::update() {
  if (state == NULL) return false;
  UIEvent evt = state->componentUpdate(id, &posSize, true, hideHover);
  return evt == UI_CLICK;
}

void UIBox::render() {
  if (state == NULL) return;
  if (renderShadow) {
    DrawRectangle(posSize.x - 2, posSize.y - 2, posSize.width + 6, posSize.height + 7, shadowColor);
  }
  Color activeColor = boxColor;
  if (state->uiIsHovering(id)) activeColor = boxHoverColor;
  DrawRectangleRec(posSize, activeColor);
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