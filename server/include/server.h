#pragma once

#include <memory>
#include <map>
#include <stop_token>
#include <thread>

#include "connectionhandler.h"
#include "iresumable.h"
#include "room.h"

class Server {

  private:
    std::shared_ptr<ConnectionHandler> m_conn_handler;
    int m_epfd;
    int m_shutdown_fd;
    std::map<int, std::shared_ptr<IResumable>> m_clients;
    std::jthread m_server_thread;
    std::shared_ptr<Room> m_room;

    void run_thread(std::stop_token st);

  public:
    Server();

    void run();
    void stop();
};

