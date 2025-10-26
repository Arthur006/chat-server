#pragma once

#include <memory>
#include <string>
#include <optional>

#include "coroutine.h"
#include "iresumable.h"

class Server;
class Room;

class ClientHandler: public IResumable, public std::enable_shared_from_this<ClientHandler> {

  private:
    int m_fd;
    Task m_task;
    std::string m_username;
    std::shared_ptr<Server> m_server;
    std::shared_ptr<Room> m_room;
    
    std::optional<std::string> recv();
    Task run();
    void exit();

  public:
    ClientHandler(int client_fd, std::shared_ptr<Server> server, std::shared_ptr<Room> room);
    void resume() override;
    void send(std::string message);
    std::string getUserName();
};
