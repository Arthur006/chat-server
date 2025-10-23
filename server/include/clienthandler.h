#pragma once

#include <memory>
#include <string>

#include "coroutine.h"
#include "iresumable.h"

class Server;
class Room;

class ClientHandler: public IResumable, public std::enable_shared_from_this<ClientHandler> {

  private:
    int m_fd;
    Task m_task;
    std::shared_ptr<Server> m_server;
    std::shared_ptr<Room> m_room;

    Task run();

  public:
    ClientHandler(int client_fd, std::shared_ptr<Server> server, std::shared_ptr<Room> room);
    void resume() override;
    void send(std::string message);
};
