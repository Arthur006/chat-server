#include <server.h>

#include <iostream>

constexpr int PORT = 8080;

Server::Server() {

  auto create_client = [this](int client_fd) {
    auto client = std::make_unique<ClientHandler>(client_fd);
    m_clients.push_back(std::move(client));
  };

  m_conn_handler = std::make_unique<ConnectionHandler>(PORT, create_client);
}

void Server::run() {
  std::cout << "Starting Server..." << std::endl;
  m_conn_handler->start();
}

void Server::stop() {
  std::cout << "Stopping Server..." << std::endl;

  m_conn_handler->stop();
  m_conn_handler.reset();

  for (auto& client : m_clients) {
    client->stop();
  }
  m_clients.clear();

  std::cout << "Stopped Server!" << std::endl;
}

