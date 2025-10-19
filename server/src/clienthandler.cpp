#include "clienthandler.h"

#include <iostream>
#include <format>
#include <sys/socket.h>
#include <string.h>

void ClientHandler::run(std::stop_token st) {

  std::cout << "Client Handler started on fd: " << m_client_fd << std::endl;

  constexpr int BUFF_SIZE = 1024;
  char buff[BUFF_SIZE] = {0};

  char out_buff[BUFF_SIZE];

  while (!st.stop_requested()) {

    std::cout << "Looping ClientHandler" << std::endl;

    int bytes_read = read(m_client_fd, buff, BUFF_SIZE);
    if (bytes_read <= 0) {
      break;
    }

    std::string out_str = std::format("Echo: {}", buff);

    send(m_client_fd, out_str.c_str(), out_str.length(), 0);

    memset(buff, 0, BUFF_SIZE);
  }
  std::cout << std::format("Closed handler {}", m_client_fd) << std::endl;
}

ClientHandler::ClientHandler(int client_fd):
  m_client_fd{client_fd} {
    m_handler_thread = std::jthread([this](std::stop_token st) {
      run(st);
    });
}

void ClientHandler::stop() {
  std::cout << "Stopping Client handler..." << std::endl;
  m_handler_thread.request_stop();
  shutdown(m_client_fd, SHUT_RD);
}
