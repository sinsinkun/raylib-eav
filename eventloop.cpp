#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;
using namespace DbI;

// temp function for resetting db setup for testing
void _setupDbTest(DbI::DbInterface* dbi) {
  dbi->setup_tables();
  dbi->new_blueprint("movies");
  dbi->new_blueprint("books");
  dbi->new_entity("Titanic", 1);
  dbi->new_entity("1984", 2);
  dbi->new_attr("genre", DbI::STR, true);
  dbi->new_attr("length", DbI::FLOAT, false, "hrs");
  dbi->new_attr("finished", DbI::BOOL, false);
  dbi->new_ba_link(1, 1);
  dbi->new_ba_link(1, 2);
  dbi->new_ba_link(1, 3);
  dbi->new_ba_link(2, 1);
  dbi->new_ba_link(2, 3);
  dbi->new_value(1, 1, "drama");
  dbi->new_value(2, 1, "alt history");
  dbi->new_value(1, 1, "romance");
  dbi->new_value(1, 2, "1.25");
  dbi->new_value(1, 3, "true");
}

void EventLoop::init() {
  // initialize assets
  font = LoadFont("assets/Helvetica.ttf");
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  // load db
  dbInterface.init();
  int err = dbInterface.check_tables();
  if (err != 0) {
    std::cout << "ERR: db structure is corrupted" << std::endl;
    CloseWindow();
    return;
  }
  // _setupDbTest(&dbInterface);
  EavResponse bpRes = dbInterface.get_blueprints();
  if (bpRes.code == 0) {
    std::vector<EavItem> bps = bpRes.data;
    // instantiate buttons based on categories
    for (int i=0; i<bps.size(); i++) {
      Rectangle posSize = { 50.0f + (float)i * 90.0f, 50.0f, 80.0f, 40.0f };
      EavBlueprint bp = EavBlueprint(bps[i], posSize, font);
      bp.relativeToCenter = false;
      categories.push_back(bp);
    }
  } else {
    std::cout << "ERR: could not find categories" << std::endl;
  }
}

void EventLoop::update() {
  _updateSystem();
  // update global state
  // ...
}

void EventLoop::render() {
  BeginDrawing();
    ClearBackground(BLACK);
    // draw to screen
    // ...
    UIEvent e = NO_EVENT;
    // draw category buttons
    for (EavBlueprint bp : categories) {
      UIEvent evt = bp.render(screenCenter, mousePos);
      if (evt > e) e = evt;
      if (evt == BTN_CLICK) {
        std::cout << "Clicked btn " << bp.id << std::endl;
      }
    }
    // change cursor based on ui event
    if (e == BTN_HOVER || e == BTN_CLICK) {
      SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
    // draw FPS overlay
    _drawFps();
  EndDrawing();
}

void EventLoop::cleanup() {
  // destroy instantiated resources
  dbInterface.disconnect();
}

void EventLoop::_updateSystem() {
  fps = GetFPS();
  screenW = GetScreenWidth();
  screenH = GetScreenHeight();
  elapsed = GetTime();
  mousePos = GetMousePosition();
  screenCenter = { (float)screenW/2, (float)screenH/2 };
}

void EventLoop::_drawFps() {
  std::string fpst = std::to_string(fps);
  std::string fpstxt = "FPS: ";
  fpstxt.append(fpst);
  Vector2 pos = { 10.0, 10.0 };
  DrawTextEx(font, fpstxt.c_str(), pos, 18.0, 0.0, GREEN);
}
