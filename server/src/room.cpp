#include "room.h"
#include <algorithm>
#include <format>

void Room::addToRoom(std::shared_ptr<ClientHandler> client) {
  m_clients.push_back(client);
}

void Room::removeFromRoom(std::shared_ptr<ClientHandler> client) {
  auto rem_it = std::remove(m_clients.begin(), m_clients.end(), client);
  m_clients.erase(rem_it);
}

void Room::broadcast(const std::shared_ptr<ClientHandler> &sender, std::string message) {

  for(const auto &client : m_clients) {
    if (client == sender)
      continue;

    client->send(std::format("{0}: {1}", sender->getUserName(), message));
  }
}
