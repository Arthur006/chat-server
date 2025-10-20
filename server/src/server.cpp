#include <server.h>

#include <iostream>
#include <format>
#include <sys/epoll.h>

constexpr int PORT = 8080;

Server::Server() {

  constexpr int epoll_flags{0};
  m_epfd = epoll_create1(epoll_flags);

  auto create_client = [this](int client_fd) {
    struct epoll_event event = {
      .events = EPOLLIN,
      .data = {
        .fd = client_fd,
      },
    };
    epoll_ctl(m_epfd, EPOLL_CTL_ADD, client_fd, &event);
    // auto client = std::make_unique<ClientHandler>(client_fd);
    // m_clients.push_back(std::move(client));
  };

  m_conn_handler = std::make_unique<ConnectionHandler>(PORT, create_client);
}

void Server::run() {
  std::cout << "Starting Server..." << std::endl;
  m_conn_handler->start();

  constexpr int MAX_EVENTS{10};
  struct epoll_event events[MAX_EVENTS];
  while(true) {

    auto num_events = epoll_wait(m_epfd, events, MAX_EVENTS, -1);
    if (num_events == -1) {
      std::cerr << "Server::run|Error polling fds" << std::endl;
      return;
    }

    std::cout << std::format("Server::run|{0} events triggered", num_events) << std::endl;
    for (int i = 0; i < num_events; i++) {
      auto event = events[i];
      auto fd = event.data.fd;

      constexpr int BUFF_SIZE = 50;
      char buff[BUFF_SIZE] = {0};
      auto num_read = read(fd, buff, BUFF_SIZE);

      auto foo = std::string(buff,num_read);
      std::cout << "Server::run|Received|" << foo << std::endl;
    }
  }
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

