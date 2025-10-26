#pragma once

#include "clienthandler.h"
#include <memory>
#include <string>
#include <vector>

class Room {
  private:
    std::vector<std::shared_ptr<ClientHandler>> m_clients;

  public:
    void addToRoom(std::shared_ptr<ClientHandler> client);
    void removeFromRoom(std::shared_ptr<ClientHandler> client);
    void broadcast(const std::shared_ptr<ClientHandler> &sender, std::string message);
};
