#include "room.h"
#include "iostream"
#include <algorithm>

void Room::addToRoom(std::shared_ptr<ClientHandler> client) {
  m_clients.push_back(client);
}

void Room::removeFromRoom(std::shared_ptr<ClientHandler> client) {
  auto rem_it = std::remove(m_clients.begin(), m_clients.end(), client);
  m_clients.erase(rem_it);
}

void Room::broadcast(std::string message) {
  std::cout << "Room::broadcast|Broadcasting to clients\n";

  for(const auto &client : m_clients) {
    std::cout << "Room::broadcast|Sending\n";
    client->send(message);
  }
}
