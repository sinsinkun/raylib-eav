#include <iostream>
#include <sqlite3.h>
#include "app.hpp"

using namespace App;

void DbInterface::init() {
  std::cout << "Opening db at: " + db_path << std::endl;
  int rcode = sqlite3_open(db_path.c_str(), &db);
  std::string srcode = std::to_string(rcode);
  if (rcode) {
    sqlite3_close(db);
    std::cout << "Error: Could not open sqlite db (" + srcode + ")" << std::endl;
  } else {
    std::cout << "Opened " + db_path + " (" + srcode + ")" << std::endl;
  }
}

void DbInterface::disconnect() {
  int rcode = sqlite3_close(db);
  std::string srcode = std::to_string(rcode);
  std::cout << "Closed db (" + srcode + ")" << std::endl;
}