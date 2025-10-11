#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
constexpr int PORT = 8080;

int main(int argc, char** argv) {
  std::cout << "Starting server..." << std::endl;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Could not create socket");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT),
    .sin_addr = INADDR_ANY,
  };
  socklen_t address_length = sizeof(address);

  if (bind(server_fd, (sockaddr *)&address, address_length) < 0) {
    perror("Bind");
    return 1;
  }

  if (listen(server_fd, 1) < 0) {
    perror("Listen");
    return 1;
  }

  std::cout << "Server listening on port " << PORT << std::endl;

  int client_fd = accept(server_fd, (sockaddr *)&address, &address_length);
  if (client_fd < 0) {
    perror("Accept");
    return 1;
  }
  std::cout << "Client connected" << std::endl;

  char buffer[1024] = {0};
  auto bytes_read = read(client_fd, buffer, sizeof(buffer));
  std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;

  std::string reply = "Hello from the server";
  send(client_fd, reply.c_str(), reply.size(), 0);

  close(client_fd);
  close(server_fd);

  return 0;
}
