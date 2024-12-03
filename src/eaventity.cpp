#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

EavEntity::EavEntity(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn, DbI::DbInterface* dbi) {
  id = item.entity_id;
  name = item.entity;
  // init box
  int grabId = globalState->getNewDragId();
  box = UIBox(globalState, grabId);
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
  int b = GetRandomValue(120, 160);
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
    switch (v.value_type) {
      case DbI::INT:
        str += std::to_string(v.int_value);
        if (v.value_unit != "") str += " " + v.value_unit; 
        break;
      case DbI::FLOAT:
        str += std::to_string(v.float_value);
        if (v.value_unit != "") str += " " + v.value_unit; 
        break;
      case DbI::BOOL:
        str += v.bool_value ? "Yes" : "No";
        break;
      default:
        str += v.str_value == "" ? "-" : v.str_value;
        break;
    }
    box.body.push_back(str);
  }
}

bool EavEntity::update() {
  // update box
  return box.update();
}

void EavEntity::render() {
  box.render();
}