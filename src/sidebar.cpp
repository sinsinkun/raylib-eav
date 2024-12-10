#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

SideBar::SideBar(UIState* globalState, DbI::DbInterface* dbi, Rectangle scrn) {
  db = dbi;
  box = UIBox(globalState);
  box.posSize = { 1200.0f, 80.0f, scrn.width, scrn.height - 80.0f };
  box.hideHover = true;
  Color clr = Color { 180, 180, 180, 255 };
  box.boxColor = clr;
  box.boxHoverColor = clr;
  box.title = "Test";

  closeBtn = UIButton(
    globalState,
    Rectangle { 1200.0f - 20.0f, scrn.height/2.0f - 20.0f, 21.0f, 80.0f },
    "+"
  );
  closeBtn.fontSize = 12.0f;
  closeBtn.btnColor = clr;
  closeBtn.btnHoverColor = clr;
}

int SideBar::update() {
  if (box.state == NULL) return 0;
  // update position of box + closeBtn
  float w = box.state->screen.width;
  float h = box.state->screen.height;
  float bw = box.posSize.width;
  float x0 = box.posSize.x;
  if (open) {
    float dx = x0 - (w - bw);
    x0 = (x0 > w - bw) ? x0 - 16.0f : w - bw;
    box.posSize = Rectangle { x0, 80.0f, bw, h };
  } else {
    x0 = (x0 < w) ? x0 + 16.0f : w;
    box.posSize = Rectangle { x0, 80.0f, bw, h };
  }
  closeBtn.posSize.x = x0 - 20.0f;
  closeBtn.posSize.y = h/2.0f - 20.0f;

  // handle inputs
  bool gotoNextInput = IsKeyPressed(KEY_TAB);
  int activeIdx = -1;
  float yOffset = 120.0f;
  for (int i=0; i<inputs.size(); i++) {
    // update position
    inputs[i].updatePos(box.posSize.x, yOffset);
    yOffset += inputs[i].posSize.height + inputs[i].botMargin;
    inputs[i].update();
    // switch active input
    if (i == activeIdx) inputs[i].isActive = true;
    else if (gotoNextInput && inputs[i].isActive) {
      activeIdx = i + 1;
      inputs[i].isActive = false;
    }
  }
  // reset tabbing to first input
  if (activeIdx == inputs.size()) {
    inputs[0].isActive = true;
  }
  
  // handle buttons
  yOffset += 20.0f;
  btn1.posSize.x = box.posSize.x + 20.0f;
  btn1.posSize.y = yOffset;
  btn2.posSize.x = box.posSize.x + box.posSize.width - 120.0f;
  btn2.posSize.y = yOffset;
  btn1.update();
  btn2.update();
  if (closeBtn.update()) open = !open;

  box.update();
  return 0;
}

void SideBar::render() {
  if (box.state == NULL) return;
  box.render();
  closeBtn.render();
  for (int i=0; i<inputs.size(); i++) {
    inputs[i].render();
  }
  // draw border around close btn
  Vector2 p1 = { closeBtn.posSize.x, closeBtn.posSize.y };
  Vector2 p2 = { closeBtn.posSize.x + closeBtn.posSize.width, closeBtn.posSize.y };
  Vector2 p3 = { closeBtn.posSize.x, closeBtn.posSize.y + closeBtn.posSize.height };
  Vector2 p4 = { closeBtn.posSize.x + closeBtn.posSize.width, closeBtn.posSize.y + closeBtn.posSize.height };
  DrawLineEx(p1, p2, 1.5f, box.borderColor);
  DrawLineEx(p1, p3, 1.5f, box.borderColor);
  DrawLineEx(p3, p4, 1.5f, box.borderColor);
  btn1.render();
  btn2.render();
}

void SideBar::changeDialog(DialogOption act, std::string mTxt, int bId, int eId, int aId, int vId) {
  if (box.state == NULL) return;
  clearInputs();
  action = act;
  blueprintId = bId;
  entityId = eId;
  attrId = aId;
  valueId = vId;
  // add inputs + enable/disable buttons based on action
  float x0 = box.posSize.x;
  if (act == NEW_BLUEPRINT) {
    box.title = "New Category";
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "New Category Name";
    inputs.push_back(in1);
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Add New");
    btn1.renderBorder = true;
  }
  else if (act == NEW_ATTR || act == EDIT_BLUEPRINT) {
    box.title = "Edit ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
  }
  else if (act == NEW_ENTITY) {
    box.title = "Add to ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;

    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "New Entity Name";
    in1.botMargin = 20.0f;
    // todo: load attributes for entity
    EnhancedInput in2 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f }, "-", 1);
    in2.placeholder = "Commercial";
    inputs.push_back(in1);
    inputs.push_back(in2);
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Update");
    btn1.renderBorder = true;
  }
  else if (act == NEW_VALUE || act == EDIT_ENTITY) {
    box.title = "Update ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
  }
  else {
    box.title = "Unknown Menu";
    btn1.state = NULL;
    btn2.state = NULL;
  }
}

void SideBar::cleanup() {
  clearInputs();
}

void SideBar::clearInputs() {
  for (int i=0; i<inputs.size(); i++) {
    inputs[i].cleanup();
  }
  inputs.clear();
}
