#pragma once

#include <functional>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class ConnectionHandler {
  private:
    std::function<void(int)> m_on_connect;
    int m_server_fd;
    sockaddr_in m_addr = {};
    socklen_t m_addr_len = sizeof(m_addr);
    std::jthread m_listener_thread;

    void run(std::stop_token st);

  public:
    ConnectionHandler(int port, std::function<void(int)> on_connect);
    void start();
    void stop();
};
