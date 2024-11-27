#include <raylib.h>
#include "app.hpp"

using namespace App;

void UIButton::update(Vector2 mousePos) {

}

void UIButton::render() {
  if (relativeToCenter) return;
}

void UIButton::render(Vector2 ctr) {
  if (!relativeToCenter) return;
  // calculate absolute position based on center
  int x = ctr.x + posX - width / 2;
  int y = ctr.y + posY - height / 2;
  Color clr = btnColor;
  if (isHovered) clr = btnHoverColor;
  DrawRectangle(x, y, width, height, clr);
  DrawText(text.c_str(), x + txtOffset, y + txtOffset, fontSize, txtColor);
}