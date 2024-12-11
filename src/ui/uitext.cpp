#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include "ui.hpp"

using namespace App;

UIText::UIText(UIState* gs, std::string txt, Rectangle b, int v) {
  state = gs;
  if (state != NULL) id = state->getNewId();
  text = txt;
  posSize = b;
  variant = v;
  // variant 0: overflow
  if (v == 0) displayText = txt;
  // variant 1: trimmed
  // variant 2: scrolling
  if (v == 1 || v == 2) {
    maxChar = maxCharForWidth(posSize.width);
    updateDisplayText(0);
  }
}

bool UIText::update() {
  if (state == NULL) return false;
  UIEvent evt = state->componentUpdate(id, &posSize);
  // scroll text on timer
  if (variant == 2) {
    dt += state->timeDelta;
    if (dt > (1.0f/speed)) {
      if (scrollIndex < text.size() + (int)(maxChar * 0.5f)) scrollIndex++;
      else scrollIndex = 1;
      updateDisplayText(scrollIndex);
      dt = 0.0f;
    }
  }
  return evt == UI_CLICK;
}

void UIText::render() {
  if (state == NULL) return;
  DrawTextEx(state->font, displayText.c_str(), Vector2 {posSize.x, posSize.y}, fontSize, 0.0f, txtColor);
}

void UIText::updateDisplayText(int start) {
  if (start == 0) {
    displayText = text;
  } else {
    int spaceC = maxChar * 0.5f;
    std::string spaces;
    for (int i=0; i< spaceC; i++) {
      spaces += ' ';
    }
    std::string tmp = text + spaces;
    std::string h1;
    std::string h2;
    for (int i=0; i<tmp.size(); i++) {
      if (i < start) h1 += tmp[i];
      else h2 += tmp[i];
    }
    displayText = h2 + h1;
  }
  displayText = displayText.substr(0, maxChar);
}

int UIText::maxCharForWidth(float w) {
  std::string s = "a";
  Vector2 txtSize = MeasureTextEx(state->font, s.c_str(), fontSize, 0.0f);
  while (txtSize.x < w && s.size() < 9999) {
    s += 'a';
    txtSize = MeasureTextEx(state->font, s.c_str(), fontSize, 0.0f);
  }
  return s.size();
}