#pragma once

#include <stop_token>
#include <thread>

class ClientHandler {
  private:
    int m_client_fd;
    std::stop_source m_st_source;
    std::jthread m_handler_thread;

    void run(std::stop_token st);

  public:
    ClientHandler(int client_fd);

    void stop();
};
