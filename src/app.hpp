#include <vector>
#include <string>
#include <raylib.h>
#include "dbi.hpp"

namespace App {
  // generic UI utils
  enum UIEvent { UI_NONE, UI_HOVER_INVIS, UI_HOVER, UI_HOLD, UI_CLICK };
  enum MouseState { MOUSE_NONE, MOUSE_OVER, MOUSE_DOWN, MOUSE_HOLD, MOUSE_UP };
  // generic UI classes
  class UIState {
    public:
      Vector2 screenCenter = { 0.0f, 0.0f };
      Font font = GetFontDefault();
      float timeDelta = 0.0f;
      Vector2 mousePos = { 0.0f, 0.0f };
      Vector2 mouseDelta = { 0.0f, 0.0f };
      MouseState mouseState = MOUSE_NONE;
      MouseState rMouseState = MOUSE_NONE;
      UIEvent uiEvent = UI_NONE;
      int activeDragId = -1;
      bool clickActionAvailable = true;
      bool rClickActionAvailable = true;
      int getNewId();
      void update();
    private:
      int _uiId = 1;
  };
  class UIInput {
    public:
      UIInput(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        _mask = LoadRenderTexture(posSize.width, posSize.height);
      }
      UIInput(UIState* globalState, int sharedDragId, Rectangle bounds) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        if (sharedDragId > 0) dragId = sharedDragId;
        posSize = bounds;
        _mask = LoadRenderTexture(posSize.width, posSize.height);
      }
      UIState* state = NULL;
      int id = 0;
      int dragId = 0;
      Rectangle posSize = { 0.0f, 0.0f, 200.0f, 30.0f };
      std::string placeholder = "Type Input Here";
      std::string input = "";
      int maxInputSize = 255;
      float fontSize = 18.0f;
      bool isActive = false;
      bool disabled = false;
      Color boxColor = Color { 140, 140, 140, 255 };
      Color boxHoverColor = Color { 160, 160, 160, 255 };
      Color boxActiveColor = Color { 210, 200, 240, 255 };
      Color shadowColor = Color { 0, 0, 0, 100 };
      Color borderColor = Color { 0, 0, 0, 180 };
      Color txtColor = BLACK;
      Color placeholderColor = DARKGRAY;
      bool update();
      void render();
      void cleanup();
    private:
      RenderTexture2D _mask;
      Color _activeColor = boxColor;
      Vector2 _txtPos { 5.0f, 5.0f };
      float _bkspCooldown = 0.0f;
      int _blinkState = 0;
      float _blinkTimer = 0.0f;
      void _updateTextPos();
  };
  class UIButton {
    public:
      UIButton(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
      }
      UIButton(UIState* globalState, Rectangle posSizeIn, std::string textIn) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        text = textIn;
        posSize = posSizeIn;
        Vector2 txtDim = MeasureTextEx(state->font, text.c_str(), fontSize, 0.0);
        if (txtDim.x > posSize.width) posSize.width = txtDim.x + 10.0f;
        if (txtDim.y > posSize.height) posSize.height = txtDim.y + 10.0f;
      }
      UIButton(UIState* globalState, int sharedDragId) : UIButton(globalState) {
        if (sharedDragId > 0) dragId = sharedDragId;
      }
      UIButton(UIState* globalState, Rectangle posSizeIn, std::string textIn, int sharedDragId) : UIButton(globalState, posSizeIn, textIn) {
        if (sharedDragId > 0) dragId = sharedDragId;
      }
      UIState* state = NULL;
      int id = 0;
      int dragId = 0;
      std::string text = "";
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      float fontSize = 18.0f;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = Color { 220, 220, 220, 255 };
      Color btnDownColor = Color { 230, 230, 240, 255 };
      Color txtColor = BLACK;
      bool update();
      void render();
    private:
      Vector2 _txtPos { 0.0f, 0.0f };
      Color _activeColor = btnColor;
  };
  class UIBox {
    public:
      UIBox(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
      }
      UIBox(UIState* globalState, int sharedDragId) : UIBox(globalState) {
        if (sharedDragId > 0) dragId = sharedDragId;
      }
      UIState* state = NULL;
      int id = 0;
      int dragId = 0;
      std::string title = "";
      std::vector<std::string> body;
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      float titleFontSize = 18.0f;
      float bodyFontSize = 16.0f;
      bool renderShadow = true;
      bool renderBorder = true;
      Color boxColor = LIGHTGRAY;
      Color boxHoverColor = Color { 220, 220, 220, 255 };
      Color shadowColor = Color { 0, 0, 0, 50 };
      Color borderColor = Color { 0, 0, 0, 140 };
      Color txtColor = BLACK;
      bool update();
      void render();
    private:
      Color _activeColor = boxColor;
  };
  // specific use
  enum DialogOption {
    NO_ACTION, NEW_BLUEPRINT, NEW_ENTITY, NEW_ATTR, NEW_VALUE, NEW_VALUE_M,
    DEL_ENTITY, DEL_VALUE
  };
  class DialogBox {
    public:
      DialogBox() {};
      DialogBox(UIState* gState, Rectangle bounds, std::string titleIn);
      DialogOption activeDialog = NO_ACTION;
      int entityId = 0;
      int blueprintId = 0;
      int attrId = 0;
      int valueId = 0;
      bool isVisible = true;
      bool isDoubleInput = false;
      UIBox box = UIBox(NULL);
      UIInput input = UIInput(NULL);
      UIInput input2 = UIInput(NULL);
      UIButton btn = UIButton(NULL);
      UIButton btn2 = UIButton(NULL);
      UIButton closeBtn = UIButton(NULL);
      void changeDialog(DialogOption action, std::string metaText, int blueprintId, int entityId, int attrId, int valueId);
      bool update();
      void render();
      void cleanup();
  };
  enum OptionsParent { OP_NONE, OP_BLUEPRINT, OP_ENTITY };
  class OptionsMenu {
    public:
      OptionsMenu() {};
      OptionsMenu(UIState* globalState, std::vector<std::string> options, OptionsParent parent);
      bool isVisible = false;
      int variant = 0;
      OptionsParent parent = OP_NONE;
      UIBox box = UIBox(NULL);
      UIButton btn1 = UIButton(NULL);
      UIButton btn2 = UIButton(NULL);
      UIButton btn3 = UIButton(NULL);
      void open();
      bool update();
      void render();
  };
  class EavBlueprint {
    public:
      EavBlueprint(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn) {
        id = item.blueprint_id;
        name = item.blueprint;
        btn = UIButton(globalState, posSizeIn, item.blueprint);
      }
      int id = 0;
      std::string name = "";
      UIButton btn = UIButton(NULL);
      bool update() {
        // right click handle
        return btn.update();
      }
      void render() {
        btn.render();
      }
  };
  class EavEntity {
    public:
      EavEntity(UIState* globalState, DbI::EavItem item, Rectangle posSizeIn, DbI::DbInterface* dbi);
      int blueprintId = 0;
      int id = 0;
      int dragId = 0;
      std::string name = "";
      std::vector<DbI::EavItem> values;
      UIBox box = UIBox(NULL);
      void fillBody();
      bool update();
      void render();
  };
  class EventLoop {
    public:
      // global states
      int screenW = 0;
      int screenH = 0;
      Vector2 screenCenter = { 0.0f, 0.0f };
      Vector2 mousePos = { 0.0f, 0.0f };
      void* grabbedObject = NULL;
      int fps = 0;
      double elapsed = 0.0;
      Color bgColor = Color { 35, 35, 40, 255 };
      UIState uiGlobal;
      // data objects
      DbI::DbInterface dbInterface;
      std::vector<EavBlueprint> categories;
      std::vector<EavEntity> entities;
      DialogBox dialog;
      OptionsMenu menu;
      // methods
      void init();
      void update();
      void render();
      void cleanup();
    private:
      void _updateSystem();
      void _drawFps();
      void _fetchAllCategories();
      void _fetchCategory(int blueprintId);
      void _handleDialogEvent(DialogBox* dialog);
  };
}