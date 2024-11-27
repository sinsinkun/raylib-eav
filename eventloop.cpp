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
    eavCategories = bpRes.data;
    for (int i=0; i<eavCategories.size(); i++) {
      EavItem item = eavCategories[i];
      int id = item.blueprint_id * -1;
      UIButton btn = UIButton(id, -40 + 80 * i, -250, 70, 30, item.blueprint);
      btns.push_back(btn);
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
    for (UIButton btn : btns) {
      btn.render(screenCenter);
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
  screenCenter = { (float)screenW/2, (float)screenH/2 };
}

void EventLoop::_drawFps() {
  std::string fpst = std::to_string(fps);
  std::string fpstxt = "FPS: ";
  fpstxt.append(fpst);
  DrawText(fpstxt.c_str(), 10.0, 10.0, 20, GREEN);
}
