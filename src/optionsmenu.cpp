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
    btn1 = UIButton(gState, Rectangle{ 0.0f, 2.0f, 146.0f, 30.0f }, "Delete Entity");
  }
  if (parent == OP_BLUEPRINT) {
    box.posSize.height = 100.0f;
    btn1 = UIButton(gState, Rectangle{ 0.0f, 2.0f, 146.0f, 30.0f }, "New Category");
    btn2 = UIButton(gState, Rectangle{ 0.0f, 34.0f, 146.0f, 30.0f }, "New Attribute");
    btn3 = UIButton(gState, Rectangle{ 0.0f, 68.0f, 146.0f, 30.0f }, "Delete Category");
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
  btn3.posSize.y = box.posSize.y + 68.0f;
}

bool OptionsMenu::update() {
  if (!isVisible) return false;
  btn1.update();
  btn2.update();
  btn3.update();
  box.update();
  return false;
}

void OptionsMenu::render() {
  if (!isVisible) return;
  box.render();
  btn1.render();
  btn2.render();
  btn3.render();
}