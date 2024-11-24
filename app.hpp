#include <vector>
#include <string>
#include <raylib.h>
#include <sqlite3.h>

namespace App {
  class DbInterface {
    public:
      bool connected = false;
      std::string db_path = "assets/main.db";
      sqlite3* db;
      void init();
      void disconnect();
  };
  class EventLoop {
    public:
      // global states
      int screenW = 0;
      int screenH = 0;
      Vector2 screenCenter = { 0.0, 0.0 };
      int fps = 0;
      double elapsed = 0.0;
      DbInterface dbInterface;
      // methods
      void init();
      void update();
      void render();
      void cleanup();
    private:
      void _updateSystem();
      void _drawFps();
  };
}