#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

DialogBox::DialogBox(UIState* globalState, Rectangle bounds, std::string titleIn) {
  int dragId = globalState->getNewId();

  box = UIBox(globalState);
  box.posSize = bounds;
  if (box.posSize.width < 210.0f) box.posSize.width = 210.0f;
  if (box.posSize.height < 110.0f) box.posSize.height = 110.0f;
  box.title = titleIn;
  box.boxColor = LIGHTGRAY;
  box.boxHoverColor = LIGHTGRAY;
  box.hideHover = true;

  Rectangle inputBounds = { bounds.x + 5.0f, bounds.y + 35.0f, 200.0f, 30.0f };
  input = UIInput(globalState, inputBounds);

  Rectangle input2Bounds = { bounds.x + 5.0f, bounds.y + 70.0f, 200.0f, 30.0f };
  input2 = UIInput(NULL, input2Bounds);

  Color btnDefault = Color { 150, 150, 150, 255 };
  Color btnHover = Color { 160, 160, 160, 255 };
  Color btnDown = Color { 180, 180, 180, 255 };

  btn = UIButton(globalState);
  btn.posSize.x = bounds.x + 5.0f;
  btn.posSize.y = bounds.y + 75.0f;
  btn.text = "Submit";
  btn.btnColor = btnDefault;
  btn.btnHoverColor = btnHover;
  btn.btnDownColor = btnDown;

  btn2 = UIButton(NULL);
  btn2.posSize.x = bounds.x + 108.0f;
  btn2.posSize.y = bounds.y + 75.0f;
  btn2.posSize.width -= 5.0f;
  btn2.text = "?";
  btn2.btnColor = btnDefault;
  btn2.btnHoverColor = btnHover;
  btn2.btnDownColor = btnDown;

  closeBtn = UIButton(globalState);
  closeBtn.posSize = {
    bounds.x + bounds.width - 25.0f,
    bounds.y + 5.0f,
    20.0f,
    20.0f
  };
  closeBtn.text = "X";
  closeBtn.fontSize = 14.0f;
  closeBtn.btnColor = btnDefault;
  closeBtn.btnHoverColor = btnHover;
  closeBtn.btnDownColor = btnDown;
}

void DialogBox::changeDialog(DialogOption action, std::string metaText, int bId, int eId, int aId, int vId) {
  // update state
  activeDialog = action;
  blueprintId = bId;
  entityId = eId;
  attrId = aId;
  valueId = vId;
  bool doubleInput = false;
  // change title
  switch (action) {
    case NEW_BLUEPRINT:
      box.title = "New Category";
      input.placeholder = "New Category Name";
      break;
    case NEW_ENTITY:
      box.title = "Add ";
      input.placeholder = "New Entity Name";
      break;
    case NEW_ATTR:
      box.title = "Add Attr to ";
      input.placeholder = "New Attribute Name";
      input2.placeholder = "Value type";
      btn.text = "Add Single";
      btn2.text = "Add Multi";
      doubleInput = true;
      break;
    case NEW_VALUE:
      box.title = "";
      input.placeholder = "Attribute Name";
      input2.placeholder = "New Value";
      btn.text = "Update";
      btn2.text = "Add New";
      doubleInput = true;
      break;
    default:
      box.title = "Dialog Box";
      break;
  }
  // append meta text
  if (metaText.size() > 0) {
    if (metaText.size() > 23) metaText = metaText.substr(0, 20) + "...";
    box.title += metaText;
  }
  // change between double input and single input
  if (doubleInput && isDoubleInput == false) {
    isDoubleInput = true;
    btn2.state = box.state;
    if (btn2.state != NULL && btn2.id == 0) btn2.id = btn2.state->getNewId();

    input2.state = box.state;
    if (input2.state != NULL && input2.id == 0) input2.id = input2.state->getNewId();
    input2.posSize.x = box.posSize.x + 5.0f;
    input2.posSize.y = box.posSize.y + 70.0f;
    box.posSize.height += 35.0f;
    btn.posSize.y += 35.0f;
    btn2.posSize.x = box.posSize.x + 108.0f;
    btn2.posSize.y = box.posSize.y + 110.0f;
  }
  if (!doubleInput && isDoubleInput == true) {
    isDoubleInput = false;
    btn2.state = NULL;
    input2.state = NULL;
    btn.text = "Submit";
    box.posSize.height -= 35.0f;
    btn.posSize.y -= 35.0f;
    btn2.posSize.y -= 35.0f;
  }
}

bool DialogBox::update() {
  if (!isVisible) return false;
  // take keyboard input
  if (IsKeyPressed(KEY_TAB) && isDoubleInput && (input.isActive || input2.isActive)) {
    input.isActive = !input.isActive;
    input2.isActive = !input2.isActive;
  }
  // update input
  input.update();
  input2.update();
  // update btn
  bool actioned = false;
  if (btn.update()) actioned = true;
  bool click2 = btn2.update();
  if (click2 && activeDialog == NEW_ATTR) {
    activeDialog = NEW_ATTR_M;
    actioned = true;
  }
  if (click2 && activeDialog == NEW_VALUE) {
    std::cout << "btn2 click value" << std::endl;
    activeDialog = NEW_VALUE_M;
    actioned = true;
  }
  if (closeBtn.update()) isVisible = false;
  // update box
  if (box.update()) {
    std::vector<int> ids = { box.id, btn.id, btn2.id, input.id, input2.id, closeBtn.id };
    box.state->uiStartHolding(ids);
  }
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