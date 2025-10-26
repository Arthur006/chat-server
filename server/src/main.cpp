#include <iostream>

#include "server.h"

int main(int argc, char** argv) {

  std::string stop;

  auto server = std::make_shared<Server>();
  server->run();
  std::cin >> stop;
  server->stop();

  return 0;
}
