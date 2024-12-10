#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

// centered input
EnhancedInput::EnhancedInput(UIState* globalState, Rectangle bounds) : UIInput(globalState, bounds) {
  posSize.x = bounds.x + 50.0f;
  posSize.width = 250.0f;
};

// right aligned input
EnhancedInput::EnhancedInput(UIState* globalState, Rectangle bounds, std::string labl, int id) : UIInput(globalState, bounds) {
  posSize.x = bounds.x + 180.0f;
  posSize.width = 200.0f;
  label = labl;
  attrId = id;
}

// update position relative to box
void EnhancedInput::updatePos(float x0, float yOff) {
  if (attrId == 0) {
    posSize.x = x0 + 55.0f;
    posSize.y = yOff;
  } else {
    posSize.x = x0 + 150.0f;
    posSize.y = yOff;
  }
}

void EnhancedInput::render() {
  if (state == NULL) return;
  DrawTextEx(state->font, label.c_str(), { posSize.x - 140.0f, posSize.y + 5.0f }, fontSize, 0.0f, txtColor);
  UIInput::render();
}