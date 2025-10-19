#include <iostream>
#include <server.h>

constexpr int PORT = 8080;

int main(int argc, char** argv) {

  std::string stop;

  Server server;
  server.run();
  std::cin >> stop;
  server.stop();

  return 0;
}
