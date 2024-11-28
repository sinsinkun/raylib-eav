#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

UIEvent EavBlueprint::render() {
  Vector2 ctr = { 0.0f, 0.0f };
  Vector2 mp = { 0.0f, 0.0f };
  return render(ctr, mp);
}

UIEvent EavBlueprint::render(Vector2 ctr, Vector2 mPos) {
  if (!relativeToCenter) ctr = { 0.0f, 0.0f };
  // calculate absolute position based on center
  int absX = ctr.x + posSize.x - posSize.width / 2;
  int absY = ctr.y + posSize.y - posSize.height / 2;

  // calculate if btn is being hovered
  UIEvent event = NO_EVENT;
  Rectangle bounds = { (float)absX, (float)absY, posSize.width, posSize.height };
  Color clr = btnColor;

  if (CheckCollisionPointRec(mPos, bounds)) {
    event = BTN_HOVER;
    clr = btnHoverColor;
  }
  if (event == BTN_HOVER && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    event = BTN_HOLD;
    clr = btnDownColor;
  }
  if (event == BTN_HOVER && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    event = BTN_CLICK;
    clr = btnDownColor;
  }
  // draw background
  DrawRectangle(absX, absY, (int)posSize.width, (int)posSize.height, clr);
  // draw text
  if (name != "") {
    Vector2 txtDim = MeasureTextEx(font, name.c_str(), fontSize, 0.0);
    Vector2 pos = {ctr.x + posSize.x - txtDim.x / 2, ctr.y + posSize.y - txtDim.y / 2};
    DrawTextEx(font, name.c_str(), pos, fontSize, 0.0, txtColor);
  }
  return event;
}