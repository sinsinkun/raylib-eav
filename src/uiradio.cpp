#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

UIRadio::UIRadio(UIState* gs, Vector2 tl, std::string t) {
  state = gs;
  if (state != NULL) id = state->getNewId();
  topLeft = tl;
  text = t;
  // calculate w/h
  Vector2 size = MeasureTextEx(state->font, t.c_str(), fontSize, 0.0f);
  width = size.x + 16.0f;
  height = size.y;
}

bool UIRadio::update() {
  if (state == NULL) return false;
  Rectangle posSize = { topLeft.x, topLeft.y, width, height };
  UIEvent e = state->componentUpdate(id, &posSize);
  if (e == UI_CLICK) {
    on = !on;
    return true;
  }
  return false;
}

void UIRadio::render() {
  if (state == NULL) return;
  // calculate circle pos
  int cx = (int)(topLeft.x + 5.0f);
  int cy = (int)(topLeft.y + 6.0f);
  DrawCircle(cx, cy, 8.0f, WHITE);
  if (on) DrawCircle(cx, cy, 5.0f, BLACK);
  if (!text.empty()) {
    DrawTextEx(state->font, text.c_str(), Vector2{ topLeft.x + 20.0f, topLeft.y }, fontSize, 0.0f, BLACK);
  }
}