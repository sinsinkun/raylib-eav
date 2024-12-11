#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

AttrRadio::AttrRadio(
  UIState* gs, Vector2 tl, int id, std::string attr, DbI::EavValueType vt, bool am, std::string unit
) : UIRadio(gs) {
  std::string txt = attr;
  if (!unit.empty()) txt += " (" + unit + ")";
  txt += " - " + DbI::value_type_to_str(vt);
  if (am) txt += "+";
  UIRadio::text = txt;
  Vector2 size = MeasureTextEx(state->font, txt.c_str(), fontSize, 0.0f);
  UIRadio::posSize = Rectangle { tl.x, tl.y, size.x + 16.0f, size.y };
}