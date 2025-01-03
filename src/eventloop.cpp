#include <iostream>
#include <string>
#include <unordered_set>
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
  menu = OptionsMenu(&uiGlobal, OP_NONE);
  appBar = AppBar(&uiGlobal, Rectangle {0.0f, 0.0f, 1200.0f, 80.0f });
  sideBar = SideBar(&uiGlobal, &dbInterface, Rectangle { 0.0f, 0.0f, 360.0f, 700.0f });
  sideBar.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);

  Vector2 a = MeasureTextEx(uiGlobal.font, "Hello World", 18.0f, 0.0f);
  Vector2 b = MeasureTextEx(uiGlobal.font, "Hello World    ", 18.0f, 0.0f);
  std::cout << a.x << " : " << b.x << std::endl;

}

void EventLoop::update() {
  // update global state
  uiGlobal.update();
  // update all components backwards -> first click event is the last component rendered
  int menuAction = menu.update();
  if (menuAction > 0) {
    _handleOption(&menu, menuAction);
    menu.isVisible = false;
  }
  // update categories
  int sortIndex = -1;
  for (int i=categories.size()-1; i >= 0; i--) {
    // handler for "add new" button
    if (categories[i].id == -10) {
      if (categories[i].update()) {
        sideBar.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);
        sideBar.open = true;
      }
      break;
    }
    if (categories[i].update()) {
      _fetchCategory(categories[i].id);
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
    _search(appBar.searchInput.input);
  };
  int sideBarAction = sideBar.update();
  if (sideBarAction == 1) {
    _handleSideBar(&sideBar);
  } else if (sideBarAction == 3) {
    // open delete menu for attr
    menu = OptionsMenu(&uiGlobal, OP_ATTR);
    for (int i=0; i<sideBar.radios.size(); i++) {
      if (sideBar.attrId == sideBar.radios[i].attrId) {
        menu.attrId = sideBar.attrId;
        menu.metaText = sideBar.radios[i].attr;
        menu.open();
        break;
      }
    }
  }
  // update entities
  sortIndex = -1;
  for (int i=entities.size()-1; i >= 0; i--) {
    if (entities[i].update()) {
      sideBar.changeDialog(EDIT_ENTITY, entities[i].name, entities[i].blueprintId, entities[i].id, 0, 0);
      sortIndex = i;
    }
    if (uiGlobal.uiIsRClicked(entities[i].box.id)) {
      menu = OptionsMenu(&uiGlobal, OP_ENTITY);
      menu.metaText = entities[i].name;
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
    menu.render();
    errBox.render();
    // draw FPS overlay
    _drawFps();
  EndDrawing();
}

void EventLoop::cleanup() {
  // destroy instantiated resources
  appBar.cleanup();
  dbInterface.disconnect();
}

void EventLoop::_drawFps() {
  int fps = GetFPS();
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
  appBar.searchInput.clear();
  sideBar.open = false;
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

int EventLoop::_queryBuilder(std::string q, EntityQuery* eq) {
  q = trim_space(q);
  std::vector<std::string> cmprs = { ">=", "<=", ">", "<", "=", ": ", "in ", "IN " };
  for (int i=0; i<cmprs.size(); i++) {
    std::vector<std::string> cmpVec = str_split(q, cmprs[i]);
    if (cmpVec.size() == 2) {
      std::string a = trim_space(cmpVec[0]);
      std::string v = trim_space(cmpVec[1]);
      if (v == "_null") {
        eq->comparator = ATTR_NULL;
        eq->attr = a;
      }
      else if ((cmprs[i] == "in " || cmprs[i] == "IN ") && a.empty()) {
        int bpId = 0;
        eq->comparator = BP_NAMED;
        eq->blueprint = v;
      }
      else {
        if (cmprs[i] == ">=") eq->comparator = ATTR_GTE;
        else if (cmprs[i] == "<=") eq->comparator = ATTR_LTE;
        else if (cmprs[i] == ">") eq->comparator = ATTR_GT;
        else if (cmprs[i] == "<") eq->comparator = ATTR_LT;
        else if (cmprs[i] == "=") {
          eq->comparator = ATTR_EQUAL;
          if (!isValidDecimal(v)) v = "\"" + v + "\"";
        }
        else if (cmprs[i] == ": ") {
          eq->comparator = ATTR_LIKE;
          // conversions for bools
          if (v == "yes" || v == "Yes") {
            eq->comparator = ATTR_EQUAL;
            v = "\"true\"";
          }
          else if (v == "no" || v == "No") {
            eq->comparator = ATTR_EQUAL;
            v = "\"false\"";
          }
        }
        eq->attr = a;
        eq->value = v;
      }
      break;
    }
  }
  // note: entity_named is the default comparator
  if (eq->comparator == ENTITY_NAMED) {
    eq->entity = q;
  }
  return 0;
}

void EventLoop::_search(std::string query) {
  entities.clear();
  sideBar.open = false;
  // break q into EntityQuery entries
  std::vector<std::string> qs = str_split(query, ",");
  std::vector<EntityQuery> eqs;
  std::vector<std::string> cmprs = { ">=", "<=", ">", "<", "=", ": ", "@" };
  for (int i=0; i < qs.size(); i++) {
    std::string q = trim_space(qs[i]);
    EntityQuery eq;
    // extract chain
    if (i == 0) eq.chain = Q_AND;
    else if (
      (q[0] == 'a' || q[0] == 'A') && 
      (q[1] == 'n' || q[1] == 'N') && 
      (q[2] == 'd' || q[2] == 'D')
    ) {
      eq.chain = Q_AND;
      q = q.substr(3);
    }
    else if (q[0] == 'o' && q[1] == 'r') {
      eq.chain = Q_OR;
      q = q.substr(2);
    }
    else {
      errBox.setError("ERR: and/or operator expected");
      return;
    }
    // extract query params
    int errCode = _queryBuilder(q, &eq);
    if (errCode == 2) {
      errBox.setError("ERR: Category not found");
      return;
    }
    eqs.push_back(eq);
  }
  EavResponse res = dbInterface.search_entities(eqs);
  // spawn entries
  _fillEntities(&res);
  // clear active category
  for (int i=0; i<categories.size(); i++) {
    categories[i].isActive = false;
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
      return isValidInteger(strV);
    case FLOAT:
      return isValidDecimal(strV);
    case STR:
      // prevent saving special search terms
      if (strV == "_null") return false;
      else return true;
    default:
      return false;
  }
}

void EventLoop::_handleSideBar(SideBar* sb) {
  if (sb->action == NEW_BLUEPRINT || sb->action == EDIT_BLUEPRINT) {
    // validate blueprint name
    if (sb->inputs[0].input.empty()) {
      std::string msg = "Missing name";
      std::cout << msg << std::endl;
      errBox.setError(msg);
      return;
    }
    DbResponse res = DbResponse(0);
    // new blueprint
    if (sb->blueprintId == 0) {
      res = dbInterface.new_blueprint(sb->inputs[0].input);
      int blueprintId = res.data;
    } else {
      res = dbInterface.update_blueprint(sb->blueprintId, sb->inputs[0].input);
    }
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    int blueprintId = res.data;
    // validate attrs
    for (int i=0; i<sb->radios.size(); i++) {
      if (sb->radios[i].attrId == 0) {
        std::string msg = "Attr " + sb->radios[i].attr + " is invalid";
        std::cout << msg << std::endl;
        errBox.setError(msg);
        return;
      }
    }
    // new/remove ba_links
    for (int i=0; i<sb->radios.size(); i++) {
      if (sb->radios[i].on && sb->radios[i].baId == 0) {
        res = dbInterface.new_ba_link(blueprintId, sb->radios[i].attrId);
      } else if (!sb->radios[i].on && sb->radios[i].baId != 0) {
        res = dbInterface.delete_any(DbI::BA_LINK, sb->radios[i].baId);
      }
      if (res.code != 0) {
        std::cout << res.msg << std::endl;
        errBox.setError(res.msg);
        return;
      }
    }
    _fetchAllCategories();
    sb->changeDialog(EDIT_BLUEPRINT, sb->inputs[0].input, blueprintId, 0, 0, 0);
  }
  else if (sb->action == DEL_BLUEPRINT) {
    errBox.setError("ERR: Please reconsider this action");
    _fetchCategory(sb->blueprintId);
    sb->changeDialog(NO_ACTION, "-", 0, 0, 0, 0);
  }
  else if (sb->action == NEW_ATTR) {
    // get attr
    std::string attr = sb->inputs[0].input;
    // get value type
    DbI::EavValueType vt = DbI::NONE;
    if (sb->radios[0].on) vt = DbI::STR;
    else if (sb->radios[1].on) vt = DbI::INT;
    else if (sb->radios[2].on) vt = DbI::FLOAT;
    else if (sb->radios[3].on) vt = DbI::BOOL;
    // allow multiple
    bool aMul = sb->radios[5].on;
    std::string unit = sb->inputs[1].input;
    DbResponse res = DbResponse(0);
    if (sb->blueprintId != 0) {
      res = dbInterface.new_attr_for_blueprint(sb->blueprintId, attr, vt, aMul, unit);
    } else if (unit.empty()) {
      res = dbInterface.new_attr(attr, vt, aMul);
    } else {
      res = dbInterface.new_attr(attr, vt, aMul, unit);
    }
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    _fetchCategory(sb->blueprintId);
    sb->changeDialog(NO_ACTION, "-", 0, 0, 0, 0);
  }
  else if (sb->action == DEL_ATTR) {
    // delete values
    DbResponse res = dbInterface.delete_all_attr_values(sb->attrId);
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    // delete ba_links
    res = dbInterface.delete_all_attr_links(sb->attrId);
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    // delete attr
    res = dbInterface.delete_any(DbI::ATTR, sb->attrId);
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    _fetchCategory(sb->blueprintId);
    sb->changeDialog(NO_ACTION, "-", 0, 0, 0, 0);
  }
  else if (sb->action == NEW_ENTITY || sb->action == EDIT_ENTITY) {
    if (sb->blueprintId == 0) {
      std::string msg = "Missing blueprint id";
      std::cout << msg << std::endl;
      errBox.setError(msg);
      return;
    }
    DbResponse res = DbResponse(0);
    // create/update new entity
    if (sb->entityId == 0) {
      res = dbInterface.new_entity(sb->inputs[0].input, sb->blueprintId);
      sb->entityId = res.data;
    } else {
      res = dbInterface.update_entity(sb->entityId, sb->blueprintId, sb->inputs[0].input);
    }
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    // validate attr/value data
    std::string err;
    for (int i=1; i<sb->inputs.size(); i++) {
      // skip empty inputs
      if (sb->inputs[i].input.empty()) continue;
      if (sb->inputs[i].attrId == 0) {
        err = "ERR: attrId for field " + sb->inputs[i].label + " not provided";
        break;
      }
      // auto-translate booleans
      if (sb->inputs[i].valueType == DbI::BOOL) {
        if (sb->inputs[i].input == "Yes" || sb->inputs[i].input == "yes") {
          sb->inputs[i].input = "true";
        }
        if (sb->inputs[i].input == "No" || sb->inputs[i].input == "no") {
          sb->inputs[i].input = "false";
        }
      }
      if (!_valueTypeIsValid(sb->inputs[i].input, sb->inputs[i].valueType)) {
        err = "ERR: value for field " + sb->inputs[i].label + " not valid";
        break;
      }
    }
    if (!err.empty()) {
      std::cout << err << std::endl;
      errBox.setError(err);
      return;
    }
    // update attrs for entity
    for (int i=1; i<sb->inputs.size(); i++) {
      // skip empty inputs
      bool inputEmpty = sb->inputs[i].input.empty();
      int attrId = sb->inputs[i].attrId;
      int valueId = sb->inputs[i].valueId;
      if (attrId != 0 && valueId == 0 && !inputEmpty) {
        res = dbInterface.new_value(sb->entityId, attrId, sb->inputs[i].input);
      } else if (valueId != 0 && !inputEmpty) {
        res = dbInterface.update_value(valueId, sb->inputs[i].input);
      } else if (valueId != 0 && inputEmpty) {
        res = dbInterface.delete_any(DbI::VALUE, valueId);
      }
      if (res.code != 0) {
        std::cout << res.msg << std::endl;
        errBox.setError(res.msg);
        return;
      }
      if (valueId == 0) sb->inputs[i].valueId = res.data;
    }
    // refresh data
    bool isNew = true;
    for (int i=0; i<entities.size(); i++) {
      if (entities[i].id == sb->entityId) {
        entities[i].refreshBody(&dbInterface);
        isNew = false;
        break;
      }
    }
    if (isNew) _fetchCategory(sb->blueprintId);
    sb->changeDialog(EDIT_ENTITY, sb->inputs[0].input, sb->blueprintId, sb->entityId, 0, 0);
  }
  else if (sb->action == DEL_ENTITY) {
    if (sb->entityId == 0) {
      std::string msg = "Missing entity id";
      std::cout << msg << std::endl;
      errBox.setError(msg);
      return;
    }
    // delete values for entity
    DbResponse res = dbInterface.delete_all_entity_values(sb->entityId);
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    // delete entity
    res = dbInterface.delete_any(DbI::ENTITY, sb->entityId);
    if (res.code != 0) {
      std::cout << res.msg << std::endl;
      errBox.setError(res.msg);
      return;
    }
    _fetchCategory(sb->blueprintId);
    sb->changeDialog(NO_ACTION, sb->inputs[0].input, 0, 0, 0, 0);
  }
}

void EventLoop::_handleOption(OptionsMenu* menu, int action)  {
  if (menu->parent == OP_ATTR) {
    if (action == 1 && menu->attrId != 0) {
      sideBar.changeDialog(DEL_ATTR, menu->metaText, sideBar.blueprintId, sideBar.entityId, sideBar.attrId, 0);
      sideBar.open = true;
    }
  }
  if (menu->parent == OP_ENTITY) {
    if (action == 1) {
      // grab category name
      std::string metaTxt;
      for (int i=0; i<categories.size(); i++) {
        if (categories[i].id == menu->blueprintId) metaTxt = categories[i].name;
      }
      sideBar.changeDialog(NEW_ENTITY, metaTxt, menu->blueprintId, 0, 0, 0);
      sideBar.open = true;
    }
    if (action == 2 && menu->entityId != 0) {
      sideBar.changeDialog(EDIT_ENTITY, menu->metaText, menu->blueprintId, menu->entityId, 0, 0);
      sideBar.open = true;
    }
    if (action == 3 && menu->entityId != 0) {
      sideBar.changeDialog(DEL_ENTITY, menu->metaText, menu->blueprintId, menu->entityId, 0, 0);
      sideBar.open = true;
    }
  }
  if (menu->parent == OP_BLUEPRINT) {
    if (action == 1) {
      sideBar.changeDialog(NEW_BLUEPRINT, "", 0, 0, 0, 0);
      sideBar.open = true;
    }
    if (action == 2 && menu->blueprintId != 0) {
      sideBar.changeDialog(EDIT_BLUEPRINT, menu->metaText, menu->blueprintId, 0, 0, 0);
      sideBar.open = true;
    }
    if (action == 3 && menu->blueprintId != 0) {
      sideBar.changeDialog(DEL_BLUEPRINT, menu->metaText, menu->blueprintId, 0, 0, 0);
      sideBar.open = true;
    }
  }
}
#pragma endregion helpers