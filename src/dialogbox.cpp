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
  box.titleFontSize = 16.0f;
  box.boxColor = LIGHTGRAY;
  box.boxHoverColor = LIGHTGRAY;

  Rectangle inputBounds = { bounds.x + 5.0f, bounds.y + 35.0f, 200.0f, 30.0f };
  input = UIInput(globalState, dragId, inputBounds);

  Rectangle input2Bounds = { bounds.x + 5.0f, bounds.y + 70.0f, 200.0f, 30.0f };
  input2 = UIInput(NULL, dragId, input2Bounds);

  btn = UIButton(globalState, dragId);
  btn.posSize.x = bounds.x + 5.0f;
  btn.posSize.y = bounds.y + 75.0f;
  btn.text = "Submit";
  btn.btnColor = Color { 130, 130, 130, 255 };
  btn.btnHoverColor = Color { 150, 150, 150, 255 };
  btn.btnDownColor = Color { 160, 160, 160, 255 };

  btn2 = UIButton(NULL, dragId);
  btn2.posSize.x = bounds.x + 112.0f;
  btn2.posSize.y = bounds.y + 75.0f;
  btn2.posSize.width -= 10.0f;
  btn2.text = "Delete";
  btn2.btnColor = Color { 180, 110, 110, 255 };
  btn2.btnHoverColor = Color { 200, 130, 130, 255 };
  btn2.btnDownColor = Color { 220, 150, 150, 255 };

  closeBtn = UIButton(globalState, dragId);
  closeBtn.posSize = {
    bounds.x + bounds.width - 25.0f,
    bounds.y + 5.0f,
    20.0f,
    20.0f
  };
  closeBtn.text = "X";
  closeBtn.fontSize = 14.0f;
  closeBtn.btnColor = Color { 130, 130, 130, 255 };
  closeBtn.btnHoverColor = Color { 150, 150, 150, 255 };
  closeBtn.btnDownColor = Color { 160, 160, 160, 255 };
}

DialogBox::DialogBox(UIState* globalState, Rectangle bounds, std::string titleIn, bool dInput) : DialogBox(globalState, bounds, titleIn) {
  if (dInput) {
    isDoubleInput = true;
    btn2.state = box.state;
    btn2.posSize.x = box.posSize.x + 112.0f;
    btn2.posSize.y = box.posSize.y + 75.0f;

    input2.state = box.state;
    input2.posSize.x = box.posSize.x + 5.0f;
    input2.posSize.y = box.posSize.y + 70.0f;
    box.posSize.height += 35.0f;
    btn.posSize.y += 35.0f;
    btn2.posSize.y += 35.0f;
  }
}

void DialogBox::changeDialog(DialogOption action, std::string metaText, int bId, int eId, int aId, int vId) {
  // update state
  activeDialog = action;
  blueprintId = bId;
  entityId = eId;
  attrId = aId;
  valueId = vId;
  // change title
  switch (action) {
    case NEW_BLUEPRINT:
      box.title = "New Category";
      input.placeholder = "New Category Name";
      break;
    case NEW_ENTITY:
      box.title = "New Entity";
      input.placeholder = "New Entity Name";
      break;
    case NEW_ATTR:
      box.title = "New Attribute";
      input.placeholder = "New Attribute Name";
      break;
    case NEW_VALUE:
      box.title = "New Value";
      input.placeholder = "Attribute Name";
      input2.placeholder = "New Value";
      break;
    default:
      box.title = "Dialog Box";
      break;
  }
  // append meta text
  if (metaText.size() > 0) {
    if (metaText.size() > 14) metaText = metaText.substr(0, 11) + "...";
    box.title += " (" + metaText + ")";
  }
}

bool DialogBox::update() {
  if (!isVisible) return false;
  // update input
  input.update();
  input2.update();
  // update btn
  bool actioned = false;
  if (btn.update()) actioned = true;
  if (btn2.update() && activeDialog == NEW_VALUE) {
    if (input.input.size() < 1) activeDialog = DEL_ENTITY;
    else activeDialog = DEL_VALUE;
    actioned = true;
  }
  if (closeBtn.update()) isVisible = false;
  // update box
  box.update();
  return actioned;
};

void DialogBox::render() {
  if (!isVisible) return;
  box.render();
  input.render();
  input2.render();
  btn.render();
  btn2.render();
  closeBtn.render();
};

void DialogBox::cleanup() {
  input.cleanup();
}