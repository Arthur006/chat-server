#pragma once

#include <memory>
#include <vector>

#include "clienthandler.h"
#include "connectionhandler.h"

class Server {

  private:
    std::unique_ptr<ConnectionHandler> m_conn_handler;
    std::vector<std::unique_ptr<ClientHandler>> m_clients;
    int m_epfd;

  public:
    Server();

    void run();
    void stop();
};

