#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

OptionsMenu::OptionsMenu(UIState* gState, OptionsParent prt) {
  parent = prt;
  box = UIBox(gState);
  box.hideHover = true;
  box.posSize = { 0.0f, 0.0f, 150.0f, 34.0f };
  box.boxHoverColor = box.boxColor;
  if (parent == OP_ENTITY) {
    box.posSize.height = 66.0f;
    btn1 = UIButton(gState, Rectangle{ 0.0f, 2.0f, 146.0f, 30.0f }, "Update Entity");
    btn2 = UIButton(gState, Rectangle{ 0.0f, 2.0f, 146.0f, 30.0f }, "Delete Entity");
  }
  if (parent == OP_BLUEPRINT) {
    box.posSize.height = 98.0f;
    btn1 = UIButton(gState, Rectangle{ 0.0f, 2.0f, 146.0f, 30.0f }, "New Category");
    btn2 = UIButton(gState, Rectangle{ 0.0f, 34.0f, 146.0f, 30.0f }, "Edit Category");
    btn3 = UIButton(gState, Rectangle{ 0.0f, 66.0f, 146.0f, 30.0f }, "Delete Category");
  }
}

void OptionsMenu::open() {
  if (box.state == NULL) return;
  isVisible = true;
  box.posSize.x = box.state->mousePos.x;
  box.posSize.y = box.state->mousePos.y;

  btn1.posSize.x = box.posSize.x + 2.0f;
  btn1.posSize.y = box.posSize.y + 2.0f;

  btn2.posSize.x = box.posSize.x + 2.0f;
  btn2.posSize.y = box.posSize.y + 34.0f;

  btn3.posSize.x = box.posSize.x + 2.0f;
  btn3.posSize.y = box.posSize.y + 66.0f;
}

int OptionsMenu::update() {
  if (!isVisible) return 0;
  if (btn1.update()) return 1;
  if (btn2.update()) return 2;
  if (btn3.update()) return 3;
  box.update();
  if (box.state->uiIsClickedOutside(&box.posSize)) {
    isVisible = false;
  }
  return 0;
}

void OptionsMenu::render() {
  if (!isVisible) return;
  box.render();
  btn1.render();
  btn2.render();
  btn3.render();
}