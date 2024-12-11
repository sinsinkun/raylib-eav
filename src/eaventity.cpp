#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

EavEntity::EavEntity(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn, DbI::DbInterface* dbi) {
  blueprintId = item.blueprint_id;
  id = item.entity_id;
  name = item.entity;
  // init box
  box = UIBox(globalState);
  box.title = item.entity;
  box.posSize = posSizeIn;
  Vector2 txtDim = MeasureTextEx(globalState->font, box.title.c_str(), box.titleFontSize, 0.0);
  if (txtDim.x > box.posSize.width) box.posSize.width = txtDim.x + 10.0f;
  if (txtDim.y > box.posSize.height) box.posSize.height = txtDim.y + 10.0f;
  // fetch values
  if (dbi == NULL) return;
  DbI::EavResponse vRes = dbi->get_entity_values(item.entity_id);
  // randomly generate box color
  int r = GetRandomValue(160, 250);
  int g = GetRandomValue(160, 220);
  int b = GetRandomValue(120, 240);
  box.boxColor = Color { (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
  box.boxHoverColor = Color { 
    (unsigned char)(std::min(r + 20, 255)),
    (unsigned char)(std::min(g + 20, 255)),
    (unsigned char)b,
    255
  };
  // save values
  if (vRes.code == 0) {
    values = vRes.data;
    fillBody();
  }
}

void EavEntity::fillBody() {
  box.body.clear();
  for (int i=0; i<values.size(); i++) {
    DbI::EavItem v = values[i];
    std::string str = v.attr + ": ";
    if (v.value_type == DbI::BOOL && v.bool_value) {
      str += "Yes";
    } else if (v.value_type == DbI::BOOL && !v.bool_value) {
      str += v.str_value.empty() ? "-" : "No";
    } else {
      str += v.str_value.empty() ? "-" : v.str_value;
      if (v.value_unit != "") str += " " + v.value_unit;
    }
    // cut long strings down
    if (str.length() * box.bodyFontSize > box.posSize.width * 2.0f) {
      int maxCharsPerRow = (2.0f * box.posSize.width) / box.bodyFontSize;
      std::vector<std::string> strArr = str_split_length(str, maxCharsPerRow);
      for (std::string s : strArr) box.body.push_back(s);
    } else {
      box.body.push_back(str);
    }
  }
}

bool EavEntity::update() {
  // update box
  bool clicked = box.update();
  if (clicked) box.state->uiStartHolding(box.id);
  return clicked;
}

void EavEntity::render() {
  box.render();
}