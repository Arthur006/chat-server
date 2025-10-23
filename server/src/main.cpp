#include <iostream>
#include <server.h>

constexpr int PORT = 8080;

int main(int argc, char** argv) {

  std::string stop;

  auto server = std::make_shared<Server>();
  server->run();
  std::cin >> stop;
  std::cout << "main - Stopping server" << std::endl;
  server->stop();

  return 0;
}
