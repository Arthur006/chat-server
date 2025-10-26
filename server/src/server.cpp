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

    auto client = std::make_shared<ClientHandler>(client_fd, shared_from_this(), m_room);

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
    if (num_events == -1)
      return;

    for (int i = 0; i < num_events; i++) {

      auto event = events[i];
      auto fd = event.data.fd;

      if (fd == m_shutdown_fd) {
        close(m_epfd);
        break;
      }

      auto task = m_clients[fd];
      if (task) {
        task->resume();
      }
    }
  }
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
  constexpr uint64_t one = 1;
  int res = write(m_shutdown_fd, &one, sizeof(one));

  std::cout << "Stopped Server" << std::endl;
}

void Server::remove(const int &client_fd) {
  m_clients.erase(client_fd);

  struct epoll_event event;
  epoll_ctl(m_epfd, EPOLL_CTL_DEL, client_fd, &event);
}
