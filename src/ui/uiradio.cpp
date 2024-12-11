#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

UIRadio::UIRadio(UIState* gs, Vector2 tl, std::string t) {
  state = gs;
  if (state != NULL) id = state->getNewId();
  text = t;
  // calculate w/h
  Vector2 size = MeasureTextEx(state->font, t.c_str(), fontSize, 0.0f);
  posSize = Rectangle { tl.x, tl.y, size.x + 16.0f, size.y };
}

bool UIRadio::update() {
  if (state == NULL) return false;
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
  int cx = (int)(posSize.x + 5.0f);
  int cy = (int)(posSize.y + 6.0f);
  DrawCircle(cx, cy, 8.0f, WHITE);
  if (on) DrawCircle(cx, cy, 5.0f, BLACK);
  if (!text.empty()) {
    DrawTextEx(state->font, text.c_str(), Vector2{ posSize.x + 20.0f, posSize.y }, fontSize, 0.0f, txtColor);
  }
}