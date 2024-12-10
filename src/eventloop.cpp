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
  errBox = ErrorBox(&uiGlobal);
  // load db
  dbInterface.init();
  int err = dbInterface.check_tables();
  if (err != 0) {
    errBox.setError("ERR: db structure is corrupted");
    CloseWindow();
    return;
  }
  _fetchAllCategories();
  // setup universal ui components
  dialog = DialogBox(&uiGlobal, Rectangle { 980.0f, 10.0f, 210.0f, 110.0f }, "-");
  dialog.show(false, 0);
  menu = OptionsMenu(&uiGlobal, OP_NONE);
  appBar = AppBar(&uiGlobal, Rectangle {0.0f, 0.0f, 1200.0f, 80.0f });
  sideBar = SideBar(&uiGlobal, &dbInterface, Rectangle { 0.0f, 0.0f, 360.0f, 700.0f });
}

void EventLoop::update() {
  _updateSystem();
  // update global state
  uiGlobal.update();
  // update all components backwards -> first click event is the last component rendered
  int menuAction = menu.update();
  if (menuAction > 0) {
    _handleOption(&menu, menuAction);
    menu.isVisible = false;
  }
  // update dialog box
  if (dialog.update()) {
    _handleDialogEvent(&dialog);
  }
  // update categories
  int sortIndex = -1;
  for (int i=categories.size()-1; i >= 0; i--) {
    // handler for "add new" button
    if (categories[i].id == -10) {
      if (categories[i].update()) {
        dialog.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);
        dialog.show(true, 0);
        sideBar.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);
      }
      break;
    }
    if (categories[i].update()) {
      _fetchCategory(categories[i].id);
      dialog.changeDialog(NEW_ENTITY, categories[i].name, categories[i].id, 0, 0, 0);
      dialog.show(true, 0);
      sideBar.changeDialog(NEW_ENTITY, categories[i].name, categories[i].id, 0, 0, 0);
    }
    if (uiGlobal.uiIsRClicked(categories[i].btn.id)) {
      menu = OptionsMenu(&uiGlobal, OP_BLUEPRINT);
      menu.blueprintId = categories[i].id;
      menu.metaText = categories[i].name;
      menu.open();
    }
    if (categories[i].isActive) sortIndex = i;
  }
  // re-sort so active category is on top
  if (sortIndex != -1) {
    EavBlueprint b = categories[sortIndex];
    categories.erase(categories.begin() + sortIndex);
    categories.push_back(b);
  }
  // update appbar
  if (appBar.update() == 1) {
    _searchEntities(appBar.searchInput.input, 0);
  };
  sideBar.update();
  // update entities
  sortIndex = -1;
  for (int i=entities.size()-1; i >= 0; i--) {
    if (entities[i].update()) {
      dialog.changeDialog(NEW_VALUE, entities[i].name, entities[i].blueprintId, entities[i].id, 0, 0);
      dialog.show(true, 0);
      sideBar.changeDialog(NEW_VALUE, entities[i].name, entities[i].blueprintId, entities[i].id, 0, 0);
      sortIndex = i;
    }
    if (uiGlobal.uiIsRClicked(entities[i].box.id)) {
      menu = OptionsMenu(&uiGlobal, OP_ENTITY);
      menu.blueprintId = entities[i].blueprintId;
      menu.entityId = entities[i].id;
      menu.open();
    }
  }
  // re-sort entities so clicked is on top
  if (sortIndex != -1) {
    EavEntity e = entities[sortIndex];
    entities.erase(entities.begin() + sortIndex);
    entities.push_back(e);
  }
  // finalize ui updates
  errBox.update();
  uiGlobal.postUpdate();
}

void EventLoop::render() {
  BeginDrawing();
    ClearBackground(bgColor);
    // draw entities
    for (int i=0; i < entities.size(); i++) {
      entities[i].render();
    }
    sideBar.render();
    appBar.render();
    // draw category buttons
    for (int i=0; i < categories.size(); i++) {
      categories[i].render();
    }
    dialog.render();
    menu.render();
    errBox.render();
    // draw FPS overlay
    _drawFps();
  EndDrawing();
}

void EventLoop::cleanup() {
  // destroy instantiated resources
  dialog.cleanup();
  appBar.cleanup();
  dbInterface.disconnect();
}

void EventLoop::_updateSystem() {
  fps = GetFPS();
}

void EventLoop::_drawFps() {
  std::string fpst = std::to_string(fps);
  std::string fpstxt = "FPS: ";
  fpstxt.append(fpst);
  Vector2 pos = { 5.0f, uiGlobal.screen.height - 20.0f };
  Color clr = GREEN;
  if (fps < 60) clr = YELLOW;
  if (fps < 30) clr = RED;
  DrawTextEx(uiGlobal.font, fpstxt.c_str(), pos, 18.0, 0.0, clr);
}

#pragma region db actions
void EventLoop::_fetchAllCategories() {
  EavResponse bpRes = dbInterface.get_blueprints();
  if (bpRes.code == 0) {
    std::vector<EavItem> bps = bpRes.data;
    // instantiate buttons based on categories
    for (int i=0; i<bps.size(); i++) {
      Rectangle posSize = { 5.0f + (float)i * 95.0f, 50.0f, 100.0f, 30.0f };
      EavBlueprint bp = EavBlueprint(&uiGlobal, bps[i], posSize);
      categories.push_back(bp);
    }
    // add empty button if none exist
    if (categories.empty()) {
      DbI::EavItem item;
      item.blueprint_id = -10;
      item.blueprint = "Start";
      EavBlueprint addNew = EavBlueprint(&uiGlobal, item, Rectangle { 5.0f, 50.0f, 100.0f, 30.0f });
      categories.push_back(addNew);
    }
  } else {
    errBox.setError("ERR: could not find categories");
  }
}

void EventLoop::_fillEntities(EavResponse* res) {
  std::vector<EavItem> es = res->data;
  // instantiate buttons based on categories
  // calculate number of positions left to right
  int xcount = (uiGlobal.screen.width / 120) + 1;
  int yOffset = (uiGlobal.screen.height - 100) / (es.size() / xcount + 1);
  if (yOffset > 200) yOffset = 200;
  if (yOffset < 50) yOffset = 50;
  for (int i=0; i<es.size(); i++) {
    // random position near center
    int x = 20 + 100 * (i % xcount) + GetRandomValue(-10, 10);
    int y = 90 + yOffset * (int)(i / xcount) + GetRandomValue(0, 20);
    Rectangle posSize = { (float)x, (float)y, 200.0f, 250.0f };
    EavEntity e = EavEntity(&uiGlobal, es[i], posSize, &dbInterface);
    entities.push_back(e);
  }
}

void EventLoop::_fetchCategory(int blueprintId) {
  entities.clear();
  EavResponse eRes = dbInterface.get_blueprint_entities(blueprintId);
  if (eRes.code == 0) {
    _fillEntities(&eRes);
    // select active blueprint
    for (int i=0; i<categories.size(); i++) {
      if (categories[i].id == blueprintId) categories[i].isActive = true;
      else categories[i].isActive = false;
    }
  } else {
    errBox.setError("ERR: could not find category");
  }
}

void EventLoop::_searchEntities(std::string q, int altId) {
  entities.clear();
  EavResponse eRes = EavResponse({});
  if (altId > 0) eRes = dbInterface.get_entities_like(q, altId);
  else eRes = dbInterface.get_entities_like(q);
  if (eRes.code == 0) {
    _fillEntities(&eRes);
    // clear active category
    for (int i=0; i<categories.size(); i++) {
      categories[i].isActive = false;
    }
  } else {
    errBox.setError("ERR: could not find category");
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
    if (d->input.input.empty()) {
      errBox.setError("ERR: No name provided");
      return;
    }
    std::string name = trim_space(d->input.input);
    DbI::DbResponse res = dbInterface.new_blueprint(name);
    if (res.code == 0) {
      d->input.clear();
      _fetchAllCategories();
    } else {
      errBox.setError(res.msg);
    }
  } else if (dAction == NEW_ENTITY) {
    if (d->input.input.empty()) {
      errBox.setError("ERR: No name provided");
      return;
    }
    std::string name = trim_space(d->input.input);
    DbI::DbResponse res = dbInterface.new_entity(name, d->blueprintId);
    if (res.code == 0) {
      d->input.clear();
      _fetchCategory(d->blueprintId);
    } else {
      errBox.setError(res.msg);
    }
  } else if (dAction == DEL_ENTITY) {
    DbI::DbResponse res = dbInterface.delete_all_entity_values(d->entityId);
    if (res.code != 0) {
      errBox.setError(res.msg);
      return;
    }
    res = dbInterface.delete_any(DbI::EavItemType::ENTITY, d->entityId);
    if (res.code != 0) {
      errBox.setError(res.msg);
      return;
    }
    d->input.clear();
    _fetchCategory(d->blueprintId);
    d->show(false, 0);
  } else if (dAction == NEW_ATTR || dAction == NEW_ATTR_M) {
    bool allowMultiple = dAction == NEW_ATTR_M;
    std::string attrInput = trim_space(d->input.input);
    std::vector<std::string> valueInput = str_split(d->input2.input, "-");
    DbI::EavValueType vt = DbI::str_to_value_type(trim_space(valueInput.at(0)));
    std::string unit = valueInput.size() > 1 ? trim_space(valueInput.at(1)) : "";
    if (vt == DbI::NONE) {
      errBox.setError("ERR: Invalid value type");
      return;
    }
    DbI::DbResponse res = dbInterface.new_attr_for_blueprint(
      d->blueprintId, attrInput, vt, allowMultiple, unit
    );
    if (res.code == 0) {
      d->input.clear();
      d->input2.clear();
      _fetchCategory(d->blueprintId);
    } else {
      errBox.setError(res.msg);
    }
  } else if (dAction == NEW_VALUE || dAction == NEW_VALUE_M) {
    // match up attr string to attr id
    std::string attrInput = trim_space(d->input.input);
    std::string valueInput = trim_space(d->input2.input);
    if (attrInput.empty()) {
      errBox.setError("ERR: No attribute provided");
      return;
    }
    if (valueInput.empty()) {
      errBox.setError("ERR: No value provided");
      return;
    }
    EavEntity* ent = NULL;
    for (int i=0; i<entities.size(); i++) {
      if (entities[i].id == d->entityId) {
        ent = &entities[i];
        break;
      }
    }
    if (ent == NULL) {
      errBox.setError("ERR: Entity not found");
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
      errBox.setError("ERR: Attribute not found");
      return;
    }
    if (attr->value_type == DbI::BOOL) {
      if (valueInput == "yes" || valueInput == "Yes") valueInput = "true";
      if (valueInput == "no" || valueInput == "No") valueInput = "false";
    }
    // validate value type
    if (!_valueTypeIsValid(valueInput, attr->value_type)) {
      errBox.setError("ERR: Value not matching value type");
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
      if (!attr->allow_multiple) d->input.clear();
      d->input2.clear();
      // fetch new values
      DbI::EavResponse entRes = dbInterface.get_entity_values(ent->id);
      if (entRes.code == 0) {
        ent->values = entRes.data;
        ent->fillBody();
      } else {
        errBox.setError(entRes.msg);
      }
    } else {
      errBox.setError(res.msg);
    }
  } else if (dAction == DEL_VALUE) {
    // todo: match up attr string to attr id
    // todo: validate value type
    // todo: submit to db
  }
}

void EventLoop::_handleOption(OptionsMenu* menu, int action)  {
  if (menu->parent == OP_ENTITY) {
    // delete entity
    if (action == 1 && menu->entityId != 0) {
      DbI::DbResponse res = dbInterface.delete_all_entity_values(menu->entityId);
      if (res.code != 0) {
        errBox.setError(res.msg);
        return;
      }
      res = dbInterface.delete_any(DbI::EavItemType::ENTITY, menu->entityId);
      if (res.code != 0) {
        errBox.setError(res.msg);
        return;
      }
      _fetchCategory(menu->blueprintId);
    }
  }
  if (menu->parent == OP_BLUEPRINT) {
    // new blueprint
    if (action == 1) {
      dialog.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);
      sideBar.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);
    }
    // open dialog for new attr
    if (action == 2 && menu->blueprintId != 0) {
      dialog.changeDialog(NEW_ATTR, menu->metaText, menu->blueprintId, 0, 0, 0);
      sideBar.changeDialog(NEW_ATTR, menu->metaText, menu->blueprintId, 0, 0, 0);
    }
    // delete blueprint
    if (action == 3 && menu->blueprintId != 0) {
      std::string msg = "Cannot delete categories (yet): " + std::to_string(menu->blueprintId);
      errBox.setError(msg);
    }
  }
}
#pragma endregion helpers