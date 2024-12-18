#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

AttrRadio::AttrRadio(
  UIState* gs, Vector2 tl, int id, std::string a, DbI::EavValueType vt, bool am, std::string u, int id2
) : UIRadio(gs) {
  attrId = id;
  baId = id2;
  attr = a;
  valueType = vt;
  allowMultiple = am;
  unit = u;
  std::string txt = a;
  if (!u.empty()) txt += " (" + unit + ")";
  txt += " - " + DbI::value_type_to_str(vt);
  if (am) txt += "+";
  UIRadio::text = txt;
  Vector2 size = MeasureTextEx(state->font, txt.c_str(), fontSize, 0.0f);
  UIRadio::posSize = Rectangle { tl.x, tl.y, size.x + 16.0f, size.y };
}