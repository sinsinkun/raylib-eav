#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

void EventLoop::init() {
  // initialize assets
  dbInterface.init();
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
