#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

OptionsMenu::OptionsMenu(UIState* gState, std::vector<std::string> options, OptionsParent prt) {
  variant = options.size();
  parent = prt;
  box = UIBox(gState);
  box.posSize = { 0.0f, 0.0f, 150.0f, 50.0f };
  if (variant > 0) {
    UIButton btn1 = UIButton(gState);
    btn1.text = options[0];
  }
  if (variant > 1) {
    UIButton btn2 = UIButton(gState);
    btn2.text = options[1];
  }
  if (variant > 2) {
    UIButton btn3 = UIButton(gState);
    btn3.text = options[2];
  }
}

void OptionsMenu::open() {
  if (box.state == NULL) return;
  isVisible = true;
  box.posSize.x = box.state->mousePos.x;
  box.posSize.y = box.state->mousePos.y;
  // todo: update btn positions
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
  if (variant > 0) btn1.render();
  if (variant > 1) btn2.render();
  if (variant > 2) btn3.render();
}