#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

AppBar::AppBar(UIState* gs, Rectangle scrn) {
  box = UIBox(gs);
  box.posSize = scrn;
  box.hideHover = true;
  Color boxC = Color{ 29, 38, 59, 255 };
  box.boxColor = boxC;
  box.boxHoverColor = boxC;
  box.renderShadow = false;
  // search area
  float siw = scrn.width / 2.0f;
  float six = (scrn.width / 4.0f) - 45.0f;
  float sbx = scrn.width - six - 40.0f;
  searchInput = UIInput(gs, Rectangle { six, 10.0f, siw, 30.0f });
  searchInput.placeholder = "Search all categories";
  searchBtn = UIButton(gs, Rectangle { sbx, 10.0f, 90.0f, 26.0f }, "Search");
}

int AppBar::update() {
  // resize
  float w2 = box.state->screen.width / 2.0f;
  float w4 = box.state->screen.width / 4.0f;
  box.posSize.width = box.state->screen.width;
  searchInput.posSize.width = w2;
  searchInput.posSize.x = w4 - 45.0f;
  searchBtn.posSize.x = w2 + w4 - 40.0f;

  // logic updates
  int action = 0;
  if (IsKeyPressed(KEY_ENTER) && searchInput.isActive) {
    action = 1;
  }
  if (searchBtn.update()) action = 1;
  searchInput.update();
  box.update();
  return action;
}

void AppBar::render() {
  box.render();
  searchBtn.render();
  searchInput.render();
}

void AppBar::cleanup() {
  searchInput.cleanup();
}