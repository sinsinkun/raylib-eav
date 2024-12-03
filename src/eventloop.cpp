#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;
using namespace DbI;

void EventLoop::init() {
  // initialize assets
  uiGlobal.font = LoadFont("assets/Helvetica.ttf");
  SetTextureFilter(uiGlobal.font.texture, TEXTURE_FILTER_BILINEAR);
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
      EavBlueprint bp = EavBlueprint(&uiGlobal, bps[i], posSize);
      categories.push_back(bp);
    }
  } else {
    std::cout << "ERR: could not find categories" << std::endl;
  }
  // setup universal dialog box
  dialog = DialogBox(&uiGlobal, Rectangle { 295.0f, 5.0f, 210.0f, 110.0f }, "-");
  dialog.changeDialog(NEW_BLUEPRINT, 0, 0, 0, 0);
}

void EventLoop::update() {
  _updateSystem();
  // update global state
  uiGlobal.update();
  // update all components backwards -> first click event is the last component rendered
  // update dialog box
  if (dialog.update()) {
    DialogOption dAction = dialog.activeDialog;
    // todo: handle new/update events
  }
  // update entities
  int sortIndex = -1;
  for (int i=entities.size()-1; i >= 0; i--) {
    if (entities[i].update()) sortIndex = i;
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
    if (categories[i].update()) {
      uiGlobal.clickActionAvailable = false;
      entities.clear();
      EavResponse eres = dbInterface.get_blueprint_entities(categories[i].id);
      if (eres.code == 0) {
        std::vector<EavItem> es = eres.data;
        int bpId = 0;
        // instantiate buttons based on categories
        for (int i=0; i<es.size(); i++) {
          // random position near center
          int x = GetRandomValue(20, screenW - 200);
          int y = GetRandomValue(70, screenH - 250);
          Rectangle posSize = { (float)x, (float)y, 200.0f, 250.0f };
          EavEntity e = EavEntity(&uiGlobal, es[i], posSize, &dbInterface);
          entities.push_back(e);
          bpId = es[i].entity_id;
        }
        dialog.changeDialog(NEW_ENTITY, bpId, 0, 0, 0);
      }
    }
  }
  // update mouse state
  switch (uiGlobal.uiEvent) {
    case UI_HOVER:
    case UI_HOLD:
    case UI_CLICK:
      SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
      break;
    default:
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      break;
  }
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
  dialog.cleanup();
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
  DrawTextEx(uiGlobal.font, fpstxt.c_str(), pos, 18.0, 0.0, GREEN);
}
