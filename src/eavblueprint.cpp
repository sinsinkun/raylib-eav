#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

EavBlueprint::EavBlueprint(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn) {
  id = item.blueprint_id;
  name = item.blueprint;
  btn = UIButton(globalState, posSizeIn, item.blueprint);
  btn.btnColor = Color { 50, 74, 95, 255 };
  btn.btnHoverColor = Color { 76, 76, 76, 255 };
  btn.btnDownColor = Color { 35, 35, 40, 255 };
  btn.txtColor = WHITE;
}

bool EavBlueprint::update() {
  return btn.update();
}

void EavBlueprint::render() {
  // override btn render
  if (btn.state == NULL) return;
  // draw background
  Color activeColor = btn.btnColor;
  if (btn.state->uiIsClicking(id) || isActive) activeColor = btn.btnDownColor;
  else if (btn.state->uiIsHovering(id)) activeColor = btn.btnHoverColor;
  float x = btn.posSize.x;
  float y = btn.posSize.y;
  float w = btn.posSize.width;
  float h = btn.posSize.height;
  float o = w / 10.0f;
  DrawTriangle({ x + o, y }, { x, y + h }, { x + w - o, y }, activeColor);
  DrawTriangle({ x + w - o, y }, { x, y + h }, { x + w, y + h }, activeColor);
  // draw text
  if (btn.text != "") {
    Vector2 txtDim = MeasureTextEx(btn.state->font, btn.text.c_str(), btn.fontSize, 0.0);
    float txtX = x + (w - txtDim.x) / 2.0f;
    float txtY = y + (h - txtDim.y) / 2.0f;
    DrawTextEx(btn.state->font, btn.text.c_str(), Vector2{ txtX, txtY }, btn.fontSize, 0.0, btn.txtColor);
  }
}