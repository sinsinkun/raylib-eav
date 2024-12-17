#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

bool UIBox::update() {
  if (state == NULL) return false;
  UIEvent evt = state->componentUpdate(id, &posSize, true, hideHover);

  // scroll event
  hLimit = posSize.height - 20.0f;
  // approximate size of body
  float bodyH = body.size() * 20.0f;
  int maxScrollDepthi = (int)((bodyH - hLimit) / 20.0f) + 1;
  showScroll = bodyH > hLimit;
  // scroll handling
  if (showScroll && state->uiIsHovering(id) && CheckCollisionPointRec(state->mousePos, posSize)) {
    scrollDelta = GetMouseWheelMove();
  }
  if (scrollDelta != 0.0f) {
    if (bodyH > hLimit && scrollDelta > 0 && bodyStarti > 0) bodyStarti -= 1;
    if (bodyH > hLimit && scrollDelta < 0 && bodyStarti < maxScrollDepthi) bodyStarti += 1;
  }

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
  if (!title.empty()) {
    Vector2 titlePos = { posSize.x + 5.0f, posSize.y + 5.0f };
    DrawTextEx(state->font, title.c_str(), titlePos, titleFontSize, 0.0, txtColor);
  }
  if (!body.empty()) {
    float yOffset = posSize.y + 40.0f;
    int bodyEndi = body.size();
    for (int i=bodyStarti; i < body.size(); i++) {
      // prevent accessing stale data
      if (i >= body.size()) break;
      Vector2 pos = { posSize.x + 5.0f, yOffset };
      DrawTextEx(state->font, body[i].c_str(), pos, bodyFontSize, 0.0f, txtColor);
      // stop rendering at ylimit
      yOffset += bodyFontSize + 2.0f;
      if (yOffset > posSize.y + hLimit) {
        bodyEndi = i;
        break;
      }
    }
    // draw scroll bar
    if (showScroll) {
      float x = posSize.x + posSize.width - 8.0f;
      float y = posSize.y + 30.0f;
      DrawRectangle(x, y, 8, hLimit - 20.0f, Color{ 100, 100, 100, 255 });
      // draw thumb on scroll bar
      float pc = ((float)bodyEndi - (float)bodyStarti) / (float)body.size();
      float pc0 = (float)bodyStarti / (float)body.size();
      DrawRectangle(x, y + (pc0 * hLimit), 8, pc * hLimit, Color{ 50, 50, 50, 255 });
    }
  }
  if (renderBorder) {
    DrawRectangleLinesEx(posSize, 1.0f, borderColor);
  }
}