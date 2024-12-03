#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

DialogBox::DialogBox(UIState* globalState, Rectangle bounds, std::string titleIn) {
  int dragId = globalState->getNewDragId();

  box = UIBox(globalState, dragId);
  box.posSize = bounds;
  if (box.posSize.width < 210.0f) box.posSize.width = 210.0f;
  if (box.posSize.height < 110.0f) box.posSize.height = 110.0f;
  box.title = titleIn;
  box.boxColor = LIGHTGRAY;
  box.boxHoverColor = LIGHTGRAY;

  input = UIInput(globalState, dragId);
  input.posSize.x = bounds.x + 5.0f;
  input.posSize.y = bounds.y + 35.0f;

  btn = UIButton(globalState, dragId);
  btn.posSize.x = bounds.x + 5.0f;
  btn.posSize.y = bounds.y + 75.0f;
  btn.text = "Submit";
  btn.btnColor = Color { 130, 130, 130, 255 };
  btn.btnHoverColor = Color { 150, 150, 150, 255 };
  btn.btnDownColor = Color { 160, 160, 160, 255 };
}

void DialogBox::update() {
  if (!isVisible) return;
  // update input
  input.update();
  // update btn
  if (btn.update()) {
    std::cout << "Dialog value: " << input.input << std::endl;
  }
  // update box
  box.update();
  return;
};

void DialogBox::render() {
  if (!isVisible) return;
  box.render();
  input.render();
  btn.render();
};