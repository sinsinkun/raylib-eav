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
  
  // handle radios
  if (!radios.empty()) {
    yOffset = 180.0f;
    for (int i=0; i<radios.size(); i++) {
      radios[i].posSize.x = box.posSize.x + 60.0f;
      radios[i].posSize.y = yOffset;
      yOffset += 30.0f;
      radios[i].update();
    }
  }

  // handle buttons
  yOffset += 20.0f;
  int actioned = 0;
  btn1.posSize.x = box.posSize.x + 50.0f;
  btn1.posSize.y = yOffset;
  btn2.posSize.x = box.posSize.x + box.posSize.width - 150.0f;
  btn2.posSize.y = yOffset;
  if (btn1.update()) actioned = 1;
  if (btn2.update()) {
    actioned = 2;
    if (action == NEW_BLUEPRINT || action == EDIT_BLUEPRINT) {
      changeDialog(NEW_ATTR, "", blueprintId, entityId, attrId, valueId);
    }
  }
  if (closeBtn.update()) open = !open;

  box.update();
  return actioned;
}

void SideBar::render() {
  if (box.state == NULL) return;
  box.render();
  closeBtn.render();
  for (int i=0; i<inputs.size(); i++) {
    inputs[i].render();
  }
  for (int i=0; i<radios.size(); i++) {
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
    btn2.state = NULL;
  }
  else if (act == NEW_ATTR) {
    box.title = "New Attribute";
    // input for blueprint name
    EnhancedInput in1 = EnhancedInput(box.state, Rectangle{ x0, 120.0f, 400.0f, 30.0f });
    in1.placeholder = "New Attribute Name";
    in1.botMargin = 20.0f;
    inputs.push_back(in1);
    // select type
    // select allow_multiple
    // set unit
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Add New");
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
      ini.placeholder = "Enter Value Here";
      inputs.push_back(ini);
    }
    btn1 = UIButton(box.state, { x0 + 50.0f, 650.0f, 100.0f, 30.0f }, "Add New");
    btn1.renderBorder = true;
    btn2.state = NULL;
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
      ini.placeholder = "Enter Value Here";
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
          filledMultiAttrs.push_back(inf);
        }
      }
    }
    for (int i=0; i<filledMultiAttrs.size(); i++) {
      inputs.push_back(filledMultiAttrs[i]);
    }
    btn1 = UIButton(box.state, { x0 + 50.0f, 650.0f, 100.0f, 30.0f }, "Update");
    btn1.renderBorder = true;
    btn2.state = NULL;
  }
  else if (act == DEL_ENTITY) {
    box.title = "Delete ";
    box.title += mTxt.empty() ? "(Unknown)" : mTxt;
    box.title += "?";
    btn1 = UIButton(box.state, { x0 + 50.0f, 160.0f, 100.0f, 30.0f }, "Confirm");
    btn1.renderBorder = true;
    btn2.state = NULL;
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
}
