#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

void EventLoop::init() {
  // initialize assets
  dbInterface.init();
  // dbInterface.setup_tables();
  // dbInterface.new_blueprint("movies");
  // dbInterface.new_blueprint("books");
  // dbInterface.new_entity("Titanic", 1);
  // dbInterface.new_entity("1984", 2);
  // dbInterface.new_attr("genre", DbI::STR, true);
  // dbInterface.new_attr("length", DbI::FLOAT, false, "hrs");
  // dbInterface.new_attr("finished", DbI::BOOL, false);
  // dbInterface.new_ba_link(1, 1);
  // dbInterface.new_ba_link(1, 2);
  // dbInterface.new_ba_link(1, 3);
  // dbInterface.new_ba_link(2, 1);
  // dbInterface.new_ba_link(2, 3);
  // dbInterface.new_value(1, 1, "drama");
  // dbInterface.new_value(2, 1, "alt history");
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
