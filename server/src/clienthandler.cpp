#include "clienthandler.h"

#include <coroutine>
#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "room.h"

ClientHandler::ClientHandler(int client_fd, std::shared_ptr<Server> server, std::shared_ptr<Room> room):
  m_fd{client_fd},
  m_server{server},
  m_room{room}
{
  m_task = run();
}
Task ClientHandler::run() {

  std::cout << "Client Handler started on fd: " << m_fd << std::endl;

  while (true) {

    co_await std::suspend_always{};
    std::cout << "ClientHandler::run - Resumed" << std::endl;

    constexpr int BUFF_SIZE = 1024;
    char buff[BUFF_SIZE] = {0};

    int bytes_read = read(m_fd, buff, BUFF_SIZE);
    if (bytes_read <= 0) {
      m_room->removeFromRoom(shared_from_this());
      m_server->remove(m_fd);
      close(m_fd);
      co_return;
    }

    auto message = std::string(buff, bytes_read);
    m_room->broadcast(message);

    memset(buff, 0, BUFF_SIZE);
  }

  co_return;
}


void ClientHandler::resume() {
  m_task.resume();
}


void ClientHandler::send(std::string message) {
  std::cout << "ClientHandler::send|Sending on fd: " << m_fd << std::endl;

  std::cout << "ClientHandler::send|Sending " << message << std::endl;

  int ret = ::send(m_fd, message.c_str(), message.length(), 0);
  
  std::cout << "ClientHandler::send|Returned with " << ret << std::endl;
}
