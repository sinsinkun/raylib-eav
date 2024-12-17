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
  box.title = "--";
  box.shadowColor = Color { 0, 0, 0, 100 };

  closeBtn = UIButton(
    globalState,
    Rectangle { 1200.0f - 20.0f, scrn.height/2.0f - 20.0f, 21.0f, 80.0f },
    "+"
  );
  closeBtn.fontSize = 14.0f;
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

  // scroll event
  float yOffset = 120.0f;
  yLimit = box.posSize.height * 0.8;
  // approximate size of inputs 
  float aprHi = 170.0f + inputs.size() * 30.0f;
  int maxScrollDepthi = (int)((aprHi - yLimit) / 30.0f) + 1;
  float aprHr = 170.0f + radios.size() * 30.0f;
  int maxScrollDepthr = (int)((aprHr - yLimit) / 30.0f) + 1;
  // scroll handling
  if (aprHi > yLimit || aprHr > yLimit) showScroll = true;
  else showScroll = false;
  if (showScroll) {
    Rectangle scrollBounds = Rectangle{
      box.posSize.x,
      160.0f,
      box.posSize.width,
      yLimit - 130.0f,
    };
    if (box.state->uiIsHovering(box.id) && CheckCollisionPointRec(box.state->mousePos, scrollBounds)) {
      scrollDelta = GetMouseWheelMove();
    }
  }
  if (scrollDelta != 0.0f) {
    if (aprHi > yLimit && scrollDelta > 0 && inputStarti > 1) inputStarti -= 1;
    if (aprHr > yLimit && scrollDelta > 0 && radioStarti > 0) radioStarti -= 1;
    if (aprHi > yLimit && scrollDelta < 0 && inputStarti < maxScrollDepthi) inputStarti += 1;
    if (aprHr > yLimit && scrollDelta < 0 && radioStarti < maxScrollDepthr) radioStarti += 1;
  }

  // handle inputs
  int actioned = 0;
  if (!inputs.empty()) {
    bool gotoNextInput = IsKeyPressed(KEY_TAB);
    int activeIdx = -1;
    // handle input 0 separately so it always shows at the top
    inputs[0].updatePos(box.posSize.x, yOffset);
    yOffset += inputs[0].posSize.height + inputs[0].botMargin;
    inputs[0].update();
    if (gotoNextInput && inputs[0].isActive) {
      activeIdx = 1;
      inputs[0].isActive = false;
    }
    inputEndi = inputs.size();
    for (int i=inputStarti; i<inputs.size(); i++) {
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
      // stop rendering at ylimit
      if (yOffset > yLimit) {
        inputEndi = i + 1;
        break;
      }
    }
    // reset tabbing to first input
    if (activeIdx == inputs.size()) {
      inputs[0].isActive = true;
    }
  }
  
  // handle radios
  if (!radios.empty()) {
    float yOffset2 = 170.0f;
    radioEndi = radios.size();
    for (int i=radioStarti; i<radios.size(); i++) {
      radios[i].posSize.x = box.posSize.x + 60.0f;
      radios[i].posSize.y = yOffset2;
      yOffset2 += 30.0f;
      bool clicked = radios[i].update();
      if (action == NEW_ATTR && i < 4) {
        if (clicked) attrId = i + 1;
        else if (attrId != i + 1 && i < 4) radios[i].on = false;
      }
      // check right click
      if (box.state->uiIsRClicked(radios[i].id)) {
        actioned = 3;
        attrId = radios[i].attrId;
      }
      // stop rendering at ylimit
      if (yOffset2 > yLimit) {
        radioEndi = i + 1;
        break;
      }
    }
  }

  // handle buttons
  yOffset = yLimit + 40.0f;
  btn1.posSize.x = box.posSize.x + 50.0f;
  btn1.posSize.y = yOffset;
  btn2.posSize.x = box.posSize.x + box.posSize.width - 150.0f;
  btn2.posSize.y = yOffset;
  if (btn1.update()) actioned = 1;
  if (btn2.update()) {
    actioned = 2;
    if (action == NEW_BLUEPRINT || action == EDIT_BLUEPRINT) {
      changeDialog(NEW_ATTR, inputs[0].input, blueprintId, 0, 0, 0);
    } else if (action == NEW_ATTR || action == DEL_BLUEPRINT || action == DEL_ATTR || action == DEL_ENTITY) {
      changeDialog(NO_ACTION, "", 0, 0, 0, 0);
    } else if (action == NEW_ENTITY || action == EDIT_ENTITY) {
      open = false;
    }
  }
  if (closeBtn.update() && action != NO_ACTION) open = !open;

  box.update();
  return actioned;
}

void SideBar::render() {
  if (box.state == NULL) return;
  box.render();
  closeBtn.render();
  if (!inputs.empty()) inputs[0].render();
  for (int i=inputStarti; i<inputEndi; i++) {
    // prevent accessing old components
    if (i >= inputs.size()) break;
    inputs[i].render();
  }
  for (int i=radioStarti; i<radioEndi; i++) {
    // prevent accessing old components
    if (i >= radios.size()) break;
    radios[i].render();
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
  // draw scroll bar
  if (showScroll) {
    float x = box.posSize.x + box.posSize.width - 8.0f;
    float y = yLimit - 130;
    DrawRectangle(x, 160, 8, y, Color{ 100, 100, 100, 255 });
    // draw thumb on scroll bar
    float pc = 0.0f;
    float pc0 = 0.0f;
    if (!inputs.empty()) {
      pc = ((float)inputEndi - (float)inputStarti + 1.0f) / (float)inputs.size();
      pc0 = ((float)inputStarti - 1.0f) / (float)inputs.size();
    }
    if (!radios.empty()) {
      float pcr = ((float)radioEndi - (float)radioStarti) / (float)radios.size();
      float pc0r = (float)radioStarti / (float)radios.size();
      if (radios.size() > inputs.size()) {
        pc = pcr;
        pc0 = pc0r;
      }
    }
    DrawRectangle(x, 160 + (pc0 * y), 8, pc * y, Color{ 50, 50, 50, 255 });
  }
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
    // input for blueprint name
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "New Category Name";
    in1.botMargin = 20.0f;
    inputs.push_back(in1);
    // fetch all attrs
    DbI::EavResponse aRes = db->get_attrs();
    if (aRes.code != 0) {
      std::cout << aRes.msg << std::endl;
      return;
    }

    for (int i=0; i<aRes.data.size(); i++) {
      if (aRes.data[i].attr_id == 0) continue;
      // generate radio button
      AttrRadio rad = AttrRadio(
        box.state,
        Vector2{ x0 + 50.0f, 200.0f + i * 30.0f },
        aRes.data[i].attr_id,
        aRes.data[i].attr,
        aRes.data[i].value_type,
        aRes.data[i].allow_multiple,
        aRes.data[i].value_unit,
        0
      );
      rad.on = !aRes.data[i].blueprint_id == 0;
      radios.push_back(rad);
    }
    btn1 = UIButton(box.state, { x0 + 50.0f, 600.0f, 100.0f, 30.0f }, "Add New");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Add Attr");
    btn2.renderBorder = true;
  }
  else if (act == EDIT_BLUEPRINT) {
    box.title = "Edit ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
    // input for blueprint name
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "Category Name";
    in1.input = mTxt;
    in1.botMargin = 20.0f;
    inputs.push_back(in1);
    // fetch all attrs
    DbI::EavResponse aRes = db->get_attrs();
    if (aRes.code != 0) {
      std::cout << aRes.msg << std::endl;
      return;
    }
    // fetch only attached attrs
    DbI::EavResponse bRes = db->get_blueprint_attrs(blueprintId);
    if (bRes.code != 0) {
      std::cout << aRes.msg << std::endl;
      return;
    }
    for (int i=0; i<aRes.data.size(); i++) {
      if (aRes.data[i].attr_id == 0) continue;
      // update attrs list with blueprint info
      for (int j=0; j<bRes.data.size(); j++) {
        if (aRes.data[i].attr_id == bRes.data[j].attr_id) {
          aRes.data[i].ba_id = bRes.data[j].ba_id;
          aRes.data[i].blueprint = bRes.data[j].blueprint;
          aRes.data[i].blueprint_id = bRes.data[j].blueprint_id;
        }
      }
      // generate radio button
      AttrRadio rad = AttrRadio(
        box.state,
        Vector2{ x0 + 50.0f, 200.0f + i * 30.0f },
        aRes.data[i].attr_id,
        aRes.data[i].attr,
        aRes.data[i].value_type,
        aRes.data[i].allow_multiple,
        aRes.data[i].value_unit,
        aRes.data[i].ba_id
      );
      rad.on = !aRes.data[i].blueprint_id == 0;
      radios.push_back(rad);
    }
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Update");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Add Attr");
    btn2.renderBorder = true;
  }
  else if (act == DEL_BLUEPRINT) {
    box.title = "Delete ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
    box.title += "?";
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Confirm");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Cancel");
    btn2.renderBorder = true;
  }
  else if (act == NEW_ATTR) {
    box.title = "New Attribute";
    if (!mTxt.empty()) box.title += " for " + mTxt;
    // input for blueprint name
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "New Attribute Name";
    in1.botMargin = 210.0f;
    inputs.push_back(in1);
    // select type
    AttrRadio rad1 = AttrRadio(box.state, { x0, 150.0f }, "String");
    radios.push_back(rad1);
    AttrRadio rad2 = AttrRadio(box.state, { x0, 150.0f }, "Integer");
    radios.push_back(rad2);
    AttrRadio rad3 = AttrRadio(box.state, { x0, 150.0f }, "Float");
    radios.push_back(rad3);
    AttrRadio rad4 = AttrRadio(box.state, { x0, 150.0f }, "Boolean");
    radios.push_back(rad4);
    // select allow_multiple
    radios.push_back(AttrRadio(NULL));
    AttrRadio rad5 = AttrRadio(box.state, { x0, 150.0f }, "Allow multiple");
    radios.push_back(rad5);
    // set unit
    EnhancedInput in2 = EnhancedInput(box.state, Rectangle{ x0, 320.0f, 400.0f, 30.0f });
    in2.placeholder = "Attribute unit (optional)";
    inputs.push_back(in2);
    // buttons
    btn1 = UIButton(box.state, { x0 + 50.0f, 600.0f, 100.0f, 30.0f }, "Add New");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Cancel");
    btn2.renderBorder = true;
  }
  else if (act == DEL_ATTR) {
    box.title = "Delete Attr ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
    box.title += "?";
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Confirm");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Cancel");
    btn2.renderBorder = true;
  }
  else if (act == NEW_ENTITY) {
    box.title = "Add to ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
    // input for entity name
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "New Entity Name";
    in1.botMargin = 20.0f;
    inputs.push_back(in1);
    // inputs for attributes
    DbI::EavResponse aRes = db->get_blueprint_attrs(blueprintId);
    if (aRes.code != 0) {
      std::cout << aRes.msg << std::endl;
      return;
    }
    for (int i=0; i<aRes.data.size(); i++) {
      std::string label = aRes.data[i].attr;
      if (aRes.data[i].allow_multiple) label += " +";
      EnhancedInput ini = EnhancedInput(
        box.state,
        Rectangle{ x0, 120.0f + i * 35.0f, 400.0f, 30.0f },
        label,
        aRes.data[i].attr_id,
        aRes.data[i].value_type,
        0
      );
      ini.placeholder = "Enter value (" + DbI::value_type_to_str(aRes.data[i].value_type) + ")";
      inputs.push_back(ini);
    }
    btn1 = UIButton(box.state, { x0 + 50.0f, 650.0f, 100.0f, 30.0f }, "Add New");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Cancel");
    btn2.renderBorder = true;
  }
  else if (act == EDIT_ENTITY) {
    box.title = "Update Entity";
    // input for entity name
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "Entity Name";
    in1.input = mTxt;
    in1.botMargin = 20.0f;
    inputs.push_back(in1);
    // inputs for attributes
    DbI::EavResponse vRes = db->get_entity_values(entityId);
    if (vRes.code != 0) {
      std::cout << vRes.msg << std::endl;
      return;
    }
    std::vector<EnhancedInput> filledMultiAttrs;
    std::vector<int> fmaIds;
    for (int i=0; i<vRes.data.size(); i++) {
      std::string label = vRes.data[i].attr;
      if (!vRes.data[i].value_unit.empty()) label += " (" + vRes.data[i].value_unit + ")";
      EnhancedInput ini = EnhancedInput(
        box.state,
        Rectangle{ x0, 120.0f + i * 35.0f, 400.0f, 30.0f },
        label,
        vRes.data[i].attr_id,
        vRes.data[i].value_type,
        vRes.data[i].value_id
      );
      ini.placeholder = "Enter value (" + DbI::value_type_to_str(vRes.data[i].value_type) + ")";
      ini.input = vRes.data[i].str_value;
      inputs.push_back(ini);
      // append to fma items that allow multiple
      if (vRes.data[i].allow_multiple && vRes.data[i].value_id != 0) {
        bool addFma = true;
        for (int id : fmaIds) {
          if (id == vRes.data[i].attr_id) addFma = false;
        }
        if (addFma) {
          fmaIds.push_back(vRes.data[i].attr_id);
          EnhancedInput inf = EnhancedInput(
            box.state,
            Rectangle{ x0, 120.0f + i * 35.0f, 400.0f, 30.0f },
            label,
            vRes.data[i].attr_id,
            vRes.data[i].value_type,
            0
          );
          ini.placeholder = "Enter value (" + DbI::value_type_to_str(vRes.data[i].value_type) + ")";
          filledMultiAttrs.push_back(inf);
        }
      }
    }
    for (int i=0; i<filledMultiAttrs.size(); i++) {
      inputs.push_back(filledMultiAttrs[i]);
    }
    btn1 = UIButton(box.state, { x0 + 50.0f, 650.0f, 100.0f, 30.0f }, "Update");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Cancel");
    btn2.renderBorder = true;
  }
  else if (act == DEL_ENTITY) {
    box.title = "Delete ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
    box.title += "?";
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Confirm");
    btn1.renderBorder = true;
    btn2 = UIButton(box.state, { x0 + 250.0f, 600.0f, 100.0f, 30.0f }, "Cancel");
    btn2.renderBorder = true;
  }
  else {
    box.title = "--";
    btn1.state = NULL;
    btn2.state = NULL;
    open = false;
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
  radios.clear();
  inputStarti = 1;
  radioStarti = 0;
}
