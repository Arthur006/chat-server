#pragma once

#include "coroutine.h"
#include "iresumable.h"
#include <memory>
#include <string>

class Room;

class ClientHandler: public IResumable {

  private:
    int m_fd;
    Task m_task;
    std::shared_ptr<Room> m_room;

    Task run();

  public:
    ClientHandler(int client_fd, std::shared_ptr<Room> room);
    void resume() override;
    void send(std::string message);
};
