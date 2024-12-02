#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;
using namespace DbI;

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
  EavResponse bpRes = dbInterface.get_blueprints();
  if (bpRes.code == 0) {
    std::vector<EavItem> bps = bpRes.data;
    // instantiate buttons based on categories
    for (int i=0; i<bps.size(); i++) {
      Rectangle posSize = { 10.0f + (float)i * 90.0f, 30.0f, 80.0f, 30.0f };
      EavBlueprint bp = EavBlueprint(bps[i], posSize, font);
      categories.push_back(bp);
    }
  } else {
    std::cout << "ERR: could not find categories" << std::endl;
  }
  // setup test input
  dialog = DialogBox(Rectangle { 295.0f, 5.0f, 210.0f, 110.0f }, "Test Dialog", font);
}

void EventLoop::update() {
  _updateSystem();
  // update global state
  UIEvent uiState = UI_NONE;
  MouseState mState = MOUSE_NONE;
  Vector2 mDelta = GetMouseDelta();
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) mState = MOUSE_HOLD;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) mState = MOUSE_DOWN;
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) mState = MOUSE_UP;
  // update all components backwards -> first click event is the last component rendered
  bool clickActionAvailable = true;
  // update dialog box
  uiState = dialog.update(mousePos, mState, mDelta, &grabbedObject);
  // update entities
  int sortIndex = -1;
  for (int i=entities.size()-1; i >= 0; i--) {
    uiState = entities[i].update(mousePos, mState, mDelta, uiState, &grabbedObject);
    if (uiState == UI_CLICK && clickActionAvailable) {
      clickActionAvailable = false;
      sortIndex = i;
    }
  }
  // re-sort entities so clicked is on top
  if (sortIndex != -1) {
    EavEntity e = entities[sortIndex];
    entities.erase(entities.begin() + sortIndex);
    entities.push_back(e);
    // this needs to be done AFTER reordering
    if (grabbedObject == NULL) grabbedObject = &entities.back().box;
  }
  // update categories
  for (int i=categories.size()-1; i >= 0; i--) {
    if (CheckCollisionPointRec(mousePos, categories[i].posSize) && uiState == UI_NONE) {
      if (mState == MOUSE_NONE) mState = MOUSE_OVER;
      uiState = categories[i].update(mState);
      if (uiState == UI_CLICK && clickActionAvailable) {
        clickActionAvailable = false;
        entities.clear();
        EavResponse eres = dbInterface.get_blueprint_entities(categories[i].id);
        if (eres.code == 0) {
          std::vector<EavItem> es = eres.data;
          // instantiate buttons based on categories
          for (int i=0; i<es.size(); i++) {
            // random position near center
            int x = GetRandomValue(20, screenW - 200);
            int y = GetRandomValue(70, screenH - 250);
            Rectangle posSize = { (float)x, (float)y, 200.0f, 250.0f };
            EavEntity e = EavEntity(es[i], posSize, font, &dbInterface);
            entities.push_back(e);
          }
        }
      }
    } else {
      categories[i].update(MOUSE_NONE);
    }
  }
  // update mouse state
  switch (uiState) {
    case UI_HOVER:
    case UI_HOLD:
    case UI_CLICK:
      SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
      break;
    default:
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      break;
  }
  // reset hold
  if (mState == MOUSE_UP && grabbedObject != NULL) grabbedObject = NULL;
}

void EventLoop::render() {
  BeginDrawing();
    ClearBackground(bgColor);
    // draw category buttons
    for (int i=0; i < categories.size(); i++) {
      categories[i].render();
    }
    // draw entities
    for (int i=0; i < entities.size(); i++) {
      entities[i].render();
    }
    dialog.render();
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
