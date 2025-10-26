#include "connectionhandler.h"

void ConnectionHandler::run(std::stop_token st) {
  if (listen(m_server_fd, 1) < 0) {
    return;
  }

  while (!st.stop_requested()) {
    int client_fd = accept(m_server_fd, (sockaddr *)&m_addr, &m_addr_len);
    if (client_fd < 0) {
      break;
    }
    m_on_connect(client_fd);
  }
}

ConnectionHandler::ConnectionHandler(int port, std::function<void(int)> on_connect):
  m_on_connect{on_connect} {

  m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_server_fd == -1) {
    return;
  }

  int opt = 1;
  setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons(port);
  m_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(m_server_fd, (sockaddr *)&m_addr, m_addr_len) < 0) {
    return;
  }
}

void ConnectionHandler::start() {
  m_listener_thread = std::jthread([this](std::stop_token st) {
    run(st);
  });
}

void ConnectionHandler::stop() {
  m_listener_thread.request_stop();
  shutdown(m_server_fd, SHUT_RD);
}
