#include "room.h"
#include "iostream"

void Room::addToRoom(std::shared_ptr<ClientHandler> client) {
  m_clients.push_back(client);
}

void Room::broadcast(std::string message) {
  std::cout << "Room::broadcast|Broadcasting to clients\n";

  for(const auto &client : m_clients) {
    std::cout << "Room::broadcast|Sending\n";
    client->send(message);
  }
}
