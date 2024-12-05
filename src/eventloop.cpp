#include <iostream>
#include <string>
#include <raylib.h>
#include "app.hpp"
#include "font.c"

using namespace App;
using namespace DbI;

void EventLoop::init() {
  // initialize assets
  uiGlobal.font = LoadFont_Font();
  SetTextureFilter(uiGlobal.font.texture, TEXTURE_FILTER_BILINEAR);
  // load db
  dbInterface.init();
  int err = dbInterface.check_tables();
  if (err != 0) {
    std::cout << "ERR: db structure is corrupted" << std::endl;
    CloseWindow();
    return;
  }
  _fetchAllCategories();
  // setup universal dialog box
  dialog = DialogBox(&uiGlobal, Rectangle { 580.0f, 10.0f, 210.0f, 110.0f }, "-");
  dialog.isVisible = false;
  std::vector<std::string> options;
  menu = OptionsMenu(&uiGlobal, options, OP_NONE);
}

void EventLoop::update() {
  _updateSystem();
  // update global state
  uiGlobal.update();
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    menu.isVisible = false;
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    menu.open();
  }
  // update all components backwards -> first click event is the last component rendered
  menu.update();
  // update dialog box
  if (dialog.update()) {
    _handleDialogEvent(&dialog);
  }
  // update entities
  int sortIndex = -1;
  for (int i=entities.size()-1; i >= 0; i--) {
    if (entities[i].update()) {
      dialog.changeDialog(NEW_VALUE, entities[i].name, entities[i].blueprintId, entities[i].id, 0, 0);
      dialog.isVisible = true;
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
    if (categories[i].update()) {
      uiGlobal.clickActionAvailable = false;
      _fetchCategory(categories[i].id);
      dialog.changeDialog(NEW_ENTITY, categories[i].name, categories[i].id, 0, 0, 0);
      dialog.isVisible = true;
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
    menu.render();
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

#pragma region db actions
void EventLoop::_fetchAllCategories() {
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
}

void EventLoop::_fetchCategory(int blueprintId) {
  entities.clear();
  EavResponse eres = dbInterface.get_blueprint_entities(blueprintId);
  if (eres.code == 0) {
    std::vector<EavItem> es = eres.data;
    // instantiate buttons based on categories
    // calculate number of positions left to right
    int xcount = (screenW / 120) + 1;
    int yOffset = (screenH - 100) / (es.size() / xcount + 1);
    if (yOffset > 200) yOffset = 200;
    if (yOffset < 50) yOffset = 50;
    for (int i=0; i<es.size(); i++) {
      // random position near center
      int x = 20 + 100 * (i % xcount) + GetRandomValue(-10, 10);
      int y = 70 + yOffset * (int)(i / xcount) + GetRandomValue(0, 20);
      Rectangle posSize = { (float)x, (float)y, 200.0f, 250.0f };
      EavEntity e = EavEntity(&uiGlobal, es[i], posSize, &dbInterface);
      entities.push_back(e);
    }
  } else {
    std::cout << "ERR: could not find category" << std::endl;
  }
}
#pragma endregion db actions

#pragma region helpers
bool _valueTypeIsValid(std::string strV, DbI::EavValueType vType) {
  switch (vType) {
    case BOOL:
      if (strV == "true" || strV == "True" || strV == "false" || strV == "False") {
        return true;
      } else return false;
    case INT:
      try {
        int i = std::atoi(strV.c_str());
        return true;
      } catch (...) { return false; }
    case FLOAT:
      try {
        float f = std::atof(strV.c_str());
        return true;
      } catch (...) { return false; }
    case STR:
      return true;
    default:
      return false;
  }
}

void EventLoop::_handleDialogEvent(DialogBox* d) {
  DialogOption dAction = d->activeDialog;
  if (dAction == NEW_BLUEPRINT) {
    DbI::DbResponse res = dbInterface.new_blueprint(d->input.input);
    if (res.code == 0) {
      d->input.input = "";
      _fetchAllCategories();
    } else {
      std::cout << res.msg << std::endl;
    }
  } else if (dAction == NEW_ENTITY) {
    DbI::DbResponse res = dbInterface.new_entity(d->input.input, d->blueprintId);
    if (res.code == 0) {
      d->input.input = "";
      _fetchCategory(d->blueprintId);
    } else {
      std::cout << res.msg << std::endl;
    }
  } else if (dAction == DEL_ENTITY) {
    DbI::DbResponse res = dbInterface.delete_all_entity_values(d->entityId);
    if (res.code == 0) {
      res = dbInterface.delete_any(DbI::EavItemType::ENTITY, d->entityId);
      if (res.code == 0) {
        d->input.input = "";
        _fetchCategory(d->blueprintId);
        d->isVisible = false;
      } else {
        std::cout << res.msg << std::endl;
      }
    } else {
      std::cout << res.msg << std::endl;
    }
  } else if (dAction == NEW_VALUE || dAction == NEW_VALUE_M) {
    // match up attr string to attr id
    std::string attrInput = d->input.input;
    std::string valueInput = d->input2.input;
    EavEntity* ent = NULL;
    for (int i=0; i<entities.size(); i++) {
      if (entities[i].id == d->entityId) {
        ent = &entities[i];
        break;
      }
    }
    if (ent == NULL) {
      std::cout << "ERR: Entity not found" << std::endl;
      return;
    }
    EavItem* attr = NULL;
    for (int i=0; i<ent->values.size(); i++) {
      if (ent->values[i].attr == attrInput) {
        attr = &ent->values[i];
        break;
      }
    }
    if (attr == NULL) {
      std::cout << "ERR: Attribute not found" << std::endl;
      return;
    }
    // validate value type
    if (!_valueTypeIsValid(valueInput, attr->value_type)) {
      std::cout << "ERR: Value not matching value type" << std::endl;
      return;
    }
    // submit to db
    DbI::DbResponse res = DbI::DbResponse(0);
    if (attr->value_id == 0 || (dAction == NEW_VALUE_M && attr->allow_multiple)) {
      res = dbInterface.new_value(ent->id, attr->attr_id, valueInput);
    } else {
      res = dbInterface.update_value(attr->value_id, valueInput);
    }
    if (res.code == 0) {
      d->input.input = "";
      d->input2.input = "";
      // fetch new values
      DbI::EavResponse entRes = dbInterface.get_entity_values(ent->id);
      if (entRes.code == 0) {
        ent->values = entRes.data;
        ent->fillBody();
      } else {
        std::cout << entRes.msg << std::endl;
      }
    } else {
      std::cout << res.msg << std::endl;
    }
  } else if (dAction == DEL_VALUE) {
    // todo: match up attr string to attr id
    // todo: validate value type
    // todo: submit to db
  }
}
#pragma endregion helpers