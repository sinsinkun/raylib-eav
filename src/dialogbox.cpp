#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "app.hpp"

using namespace App;

DialogBox::DialogBox(Rectangle bounds, std::string titleIn, Font ft) {
  box.posSize = bounds;
  if (box.posSize.width < 210.0f) box.posSize.width = 210.0f;
  if (box.posSize.height < 110.0f) box.posSize.height = 110.0f;
  box.title = titleIn;
  box.boxColor = LIGHTGRAY;
  box.boxHoverColor = LIGHTGRAY;
  box.font = ft;

  input.posSize.x = bounds.x + 5.0f;
  input.posSize.y = bounds.y + 35.0f;
  input.font = ft;

  btn.posSize.x = bounds.x + 5.0f;
  btn.posSize.y = bounds.y + 70.0f;
  btn.displayTxt = "Submit";
  btn.font = ft;
  btn.btnColor = Color { 130, 130, 130, 255 };
  btn.btnHoverColor = Color { 150, 150, 150, 255 };
  btn.btnDownColor = Color { 160, 160, 160, 255 };
}

UIEvent DialogBox::update(Vector2 mPos, MouseState mState) {
  if (!isVisible) return UI_NONE;
  Vector2 md = Vector2{0.0f, 0.0f};
  return update(mPos, mState, md, NULL);
}

UIEvent DialogBox::update(Vector2 mousePos, MouseState mState, Vector2 mDelta, void** gObjPtr) {
  if (!isVisible) return UI_NONE;
  UIEvent uiState = UI_NONE;
  if (gObjPtr != NULL && *gObjPtr == &box) {
    box.posSize.x += mDelta.x;
    box.posSize.y += mDelta.y;
    input.posSize.x += mDelta.x;
    input.posSize.y += mDelta.y;
    btn.posSize.x += mDelta.x;
    btn.posSize.y += mDelta.y;
  }
  // update input
  if (CheckCollisionPointRec(mousePos, input.posSize)) {
    if (mState == MOUSE_NONE) mState = MOUSE_OVER;
    uiState = input.update(mState);
  } else {
    input.update(mState, true);
  }
  // update btn
  if (CheckCollisionPointRec(mousePos, btn.posSize) && uiState == UI_NONE) {
    if (mState == MOUSE_NONE) mState = MOUSE_OVER;
    uiState = btn.update(mState);
    if (uiState == UI_CLICK) {
      std::cout << "Dialog value: " << input.input << std::endl;
    }
  } else {
    btn.update(MOUSE_NONE);
  }
  // update box
  if (CheckCollisionPointRec(mousePos, box.posSize) && uiState == UI_NONE) {
    if (mState == MOUSE_NONE) mState = MOUSE_OVER;
    uiState = box.update(mState);
    if (uiState == UI_CLICK && gObjPtr != NULL && *gObjPtr == NULL) {
      *gObjPtr = &box;
    }
  } else {
    box.update(MOUSE_NONE);
  }
  return uiState;
};

void DialogBox::render() {
  if (!isVisible) return;
  box.render();
  input.render();
  btn.render();
};