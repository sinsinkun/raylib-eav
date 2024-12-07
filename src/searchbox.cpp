#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

SearchBox::SearchBox(UIState* globalState, Rectangle bounds) {
  state = globalState;

  box = UIBox(globalState);
  box.posSize = bounds;
  Color boxC = Color { 200, 200, 200, 255 };
  box.boxColor = boxC;
  box.boxHoverColor = boxC;

  input = UIInput(globalState);
  input.placeholder = "Enter query";
  input.posSize.x = bounds.x + 5.0f;
  input.posSize.y = bounds.y + 5.0f;

  Rectangle btnBounds = Rectangle{
    bounds.x + bounds.width - 85.0f,
    bounds.y + 5.0f,
    80.0f,
    30.0f
  };
  btn = UIButton(globalState, btnBounds, "Search");
  btn.btnColor = Color { 150, 150, 150, 255 };
  btn.btnHoverColor = Color { 160, 160, 160, 255 };
  btn.btnDownColor = Color { 180, 180, 180, 255 };
}

bool SearchBox::update() {
  bool actioned = false;
  if (IsKeyPressed(KEY_ENTER) && input.isActive) {
    actioned = true;
  }
  input.update();
  if (btn.update()) actioned = true;
  if (box.update()) {
    std::vector<int> ids = { box.id, btn.id, input.id };
    box.state->uiStartHolding(ids);
  }
  return actioned;
}

void SearchBox::render() {
  box.render();
  input.render();
  btn.render();
}

void SearchBox::cleanup() {
  input.cleanup();
}