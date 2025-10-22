#include <memory>
#include <server.h>

#include <iostream>
#include <format>
#include <stop_token>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "clienthandler.h"

constexpr int PORT = 8080;

Server::Server() {

  constexpr int epoll_flags{0};
  m_epfd = epoll_create1(epoll_flags);

  m_shutdown_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  std::cout << "Created shutdown fd " << m_shutdown_fd << std::endl;
  epoll_event shutdown_event{
    .events = EPOLLIN,
    .data {
      .fd = m_shutdown_fd,
    }
  };
  
  if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_shutdown_fd, &shutdown_event) < 0) {
    std::cerr << "Unable to create server\n";
    close(m_epfd);
    return;
  }

  m_room = std::make_shared<Room>();

  auto create_client = [this](int client_fd) {

    auto client = std::make_shared<ClientHandler>(client_fd, m_room);

    m_room->addToRoom(client);
    m_clients[client_fd] = client;

    struct epoll_event event = {
      .events = EPOLLIN,
      .data = {
        .fd = client_fd,
      },
    };
    epoll_ctl(m_epfd, EPOLL_CTL_ADD, client_fd, &event);
  };

  m_conn_handler = std::make_unique<ConnectionHandler>(PORT, create_client);
}

void Server::run_thread(std::stop_token st) {
  std::cout << "Starting Server..." << std::endl;
  m_conn_handler->start();

  constexpr int MAX_EVENTS{10};
  struct epoll_event events[MAX_EVENTS];
  while(!st.stop_requested()) {

    auto num_events = epoll_wait(m_epfd, events, MAX_EVENTS, -1);
    if (num_events == -1) {
      if (st.stop_requested())
        std::cout << "Server::run_thread|Stop has been requested\n";
      else
        std::cerr << "Server::run|Error polling fds" << std::endl;
      return;
    }

    std::cout << std::format("Server::run|{0} events triggered", num_events) << std::endl;
    for (int i = 0; i < num_events; i++) {

      auto event = events[i];
      auto fd = event.data.fd;

      if (fd == m_shutdown_fd) {
        std::cout << "Shutting down server\n";
        close(m_epfd);
        break;
      }

      std::cout << std::format("Found client handler {}", fd) << std::endl;
      auto task = m_clients[fd];

      std::cout << std::format("Found task") << std::endl;
      if (task) {
        std::cout << "Resuming" << std::endl;
        task->resume();
      }
    }
  }
  std::cout << "Stop was requested\n";
}

void Server::run() {
  m_server_thread = std::jthread([this](std::stop_token st) {
    run_thread(st);
  });
}

void Server::stop() {
  std::cout << "Stopping Server..." << std::endl;

  m_conn_handler->stop();
  m_conn_handler.reset();

  m_clients.clear();

  m_server_thread.request_stop();
  uint64_t one = 1;
  int res = write(m_shutdown_fd, &one, sizeof(one));
  if (res < 1)
    std::cerr << "Did not write buffer\n";

  std::cout << "Stopped Server!" << std::endl;
}

