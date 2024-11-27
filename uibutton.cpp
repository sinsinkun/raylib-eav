#include <raylib.h>
#include "app.hpp"

using namespace App;

void UIButton::update(Vector2 mousePos) {

}

void UIButton::render() {
  Vector2 ctr = {0, 0};
  render(ctr);
}

void UIButton::render(Vector2 ctr) {
  // calculate absolute position based on center
  int x = ctr.x + posX - width / 2;
  int y = ctr.y + posY - height / 2;
  Color clr = btnColor;
  if (isHovered) clr = btnHoverColor;
  // draw background
  DrawRectangle(x, y, width, height, clr);
  // draw text
  if (fontRef == NULL) {
    Font dfFont = GetFontDefault();
    Vector2 txtDim = MeasureTextEx(dfFont, text.c_str(), fontSize, 0.0);
    Vector2 pos = {ctr.x + posX - txtDim.x / 2, ctr.y + posY - txtDim.y / 2};
    DrawText(text.c_str(), pos.x, pos.y, fontSize, txtColor);
  } else {
    Vector2 txtDim = MeasureTextEx(*fontRef, text.c_str(), fontSize, 0.0);
    Vector2 pos = {ctr.x + posX - txtDim.x / 2, ctr.y + posY - txtDim.y / 2};
    DrawTextEx(*fontRef, text.c_str(), pos, fontSize, 0.0, txtColor);
  }
}