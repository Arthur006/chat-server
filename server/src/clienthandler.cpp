#include "clienthandler.h"

#include <algorithm>
#include <coroutine>
#include <sys/socket.h>
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

  send("Enter your username:\n");
  co_await std::suspend_always{};
  auto msg_opt = recv();

  if (!msg_opt) {
    exit();
    co_return;
  }

  auto msg = msg_opt.value();

  msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
  msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
  m_username = msg;

  while (true) {

    co_await std::suspend_always{};

    auto msg = recv();
    if (!msg) {
      exit();
      co_return;
    }

    m_room->broadcast(shared_from_this(), msg.value());
  }

  co_return;
}

void ClientHandler::exit() {
  m_room->removeFromRoom(shared_from_this());
  m_server->remove(m_fd);
  close(m_fd);
}

void ClientHandler::resume() {
  m_task.resume();
}

std::optional<std::string> ClientHandler::recv() {

  constexpr int BUFF_SIZE = 1024;
  char buff[BUFF_SIZE] = {0};

  int bytes_read = read(m_fd, buff, BUFF_SIZE);
  if (bytes_read <= 0) {
    return std::nullopt;
  }

  return std::string(buff, bytes_read);
}

void ClientHandler::send(std::string message) {
  ::send(m_fd, message.c_str(), message.length(), 0);
}

std::string ClientHandler::getUserName() {
  return m_username;
}
