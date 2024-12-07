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
      bool invisHover = false;
      bool clickFrame = false;
      bool rClickFrame = false;
      int hoverId = 0;
      int clickId = 0;
      int rClickId = 0;
      std::vector<int> holdIds;
      int getNewId();
      void update();
      void postUpdate();
      bool uiIsClicked(int id);
      bool uiIsRClicked(int id);
      bool uiIsHovering(int id);
      bool uiIsClicking(int id);
      bool uiIsHolding(int id);
      void uiStartHolding(int id);
      void uiStartHolding(std::vector<int> id);
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
      UIInput(UIState* globalState, Rectangle bounds) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
        posSize = bounds;
        _mask = LoadRenderTexture(posSize.width, posSize.height);
      }
      UIState* state = NULL;
      int id = 0;
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
      // Color _activeColor = boxColor;
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
      UIState* state = NULL;
      int id = 0;
      std::string text = "";
      Rectangle posSize = { 0.0f, 0.0f, 100.0f, 30.0f };
      float fontSize = 18.0f;
      Color btnColor = LIGHTGRAY;
      Color btnHoverColor = Color { 220, 220, 220, 255 };
      Color btnDownColor = Color { 230, 230, 240, 255 };
      Color txtColor = BLACK;
      bool update();
      void render();
  };
  class UIBox {
    public:
      UIBox(UIState* globalState) {
        state = globalState;
        if (state != NULL) id = state->getNewId();
      }
      UIState* state = NULL;
      int id = 0;
      bool hideHover = false;
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
  };
  // specific use
  enum DialogOption {
    NO_ACTION, NEW_BLUEPRINT, NEW_ENTITY, NEW_ATTR, NEW_ATTR_M,
    NEW_VALUE, NEW_VALUE_M, DEL_ENTITY, DEL_VALUE
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
      void show(bool onOff, int position);
      bool update();
      void render();
      void cleanup();
  };
  enum OptionsParent { OP_NONE, OP_BLUEPRINT, OP_ENTITY };
  class OptionsMenu {
    public:
      OptionsMenu() {};
      OptionsMenu(UIState* globalState, OptionsParent parent);
      bool isVisible = false;
      int blueprintId = 0;
      int entityId = 0;
      int attrId = 0;
      int valueId = 0;
      std::string metaText = "";
      OptionsParent parent = OP_NONE;
      UIBox box = UIBox(NULL);
      UIButton btn1 = UIButton(NULL);
      UIButton btn2 = UIButton(NULL);
      UIButton btn3 = UIButton(NULL);
      void open();
      int update();
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
  class ErrorBox {
    public:
      ErrorBox(UIState* globalState) {
        state = globalState;
      }
      UIState* state = NULL;
      std::string msg;
      void setError(std::string err) {
        msg = err;
        _timer = 0.01f;
      };
      void update() {
        if (_timer > 0.0f) _timer += GetFrameTime();
        if (_timer> 2.5f) {
          _timer = 0.0f;
          msg.clear();
        }
      };
      void render() {
        if (msg.empty() || state == NULL) return;
        DrawTextEx(state->font, msg.c_str(), Vector2{95.0f, 10.0f}, 18.0f, 0.0f, RED);
      };
    private:
      float _timer = 0.0f;
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
      ErrorBox errBox = ErrorBox(NULL);
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
      void _handleOption(OptionsMenu* menu, int action);
  };
  // util functions
  std::vector<std::string> str_split(std::string str, std::string delimiter);
  std::vector<std::string> str_split_length(std::string str, int size);
  std::string trim_space(std::string str);
}