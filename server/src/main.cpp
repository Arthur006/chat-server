#include <cstdio>
#include <iostream>
#include <ostream>
#include <stop_token>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
#include <string>
#include <format>
#include <vector>
#include <functional>

constexpr int PORT = 8080;

class ClientHandler {
  private:
    int m_client_fd;
    std::stop_source m_st_source;
    std::jthread m_handler_thread;

    void run(std::stop_token st) {

      std::cout << "Client Handler started on fd: " << m_client_fd << std::endl;

      constexpr int BUFF_SIZE = 1024;
      char buff[BUFF_SIZE] = {0};

      char out_buff[BUFF_SIZE];

      while (!st.stop_requested()) {

        std::cout << "Looping ClientHandler" << std::endl;

        int bytes_read = read(m_client_fd, buff, BUFF_SIZE);
        if (bytes_read <= 0) {
          break;
        }

        std::string out_str = std::format("Echo: {}", buff);
    
        send(m_client_fd, out_str.c_str(), out_str.length(), 0);
      }
      std::cout << std::format("Closed handler {}", m_client_fd) << std::endl;
    }

  public:
    ClientHandler(int client_fd):
      m_client_fd{client_fd} {
        m_handler_thread = std::jthread([this](std::stop_token st) {
          run(st);
        });
      }

    void stop() {
      std::cout << "Stopping Client handler..." << std::endl;
      m_handler_thread.request_stop();
      shutdown(m_client_fd, SHUT_RD);
    }
};

class ConnectionHandler {
  private:
    std::function<void(int)> m_on_connect;
    int m_server_fd;
    sockaddr_in m_addr = {};
    socklen_t m_addr_len = sizeof(m_addr);
    std::jthread m_listener_thread;

    void run(std::stop_token st) {
      if (listen(m_server_fd, 1) < 0) {
        return;
      }

      while (!st.stop_requested()) {
        std::cout << "Looping Connection Handler" << std::endl;
        int client_fd = accept(m_server_fd, (sockaddr *)&m_addr, &m_addr_len);
        if (client_fd < 0) {
          break;
        }
        std::cout << "Client connected" << std::endl;
        m_on_connect(client_fd);
      }

      std::cout << "Stopped Connection handler" << std::endl;
    }

  public:
    ConnectionHandler(int port, std::function<void(int)> on_connect):
      m_on_connect{on_connect} {

      m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (m_server_fd == -1) {
        return;
      }

      int opt = 1;
      setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

      m_addr.sin_family = AF_INET;
      m_addr.sin_port = htons(PORT);
      m_addr.sin_addr.s_addr = INADDR_ANY;

      if (bind(m_server_fd, (sockaddr *)&m_addr, m_addr_len) < 0) {
        return;
      }
    }

    void start() {
      m_listener_thread = std::jthread([this](std::stop_token st) {
        run(st);
      });
    }

    void stop() {
      std::cout << "Stopping Connection handler..." << std::endl;
      m_listener_thread.request_stop();
      shutdown(m_server_fd, SHUT_RD);
    }
};

class Server {
  private:
    std::unique_ptr<ConnectionHandler> m_conn_handler;
    std::vector<std::unique_ptr<ClientHandler>> m_clients;

  public:
    Server() {

      auto create_client = [this](int client_fd) {
        auto client = std::make_unique<ClientHandler>(client_fd);
        m_clients.push_back(std::move(client));
      };

      m_conn_handler = std::make_unique<ConnectionHandler>(PORT, create_client);
    }

    void run() {
      std::cout << "Starting Server..." << std::endl;
      m_conn_handler->start();
    }

    void stop() {
      std::cout << "Stopping Server..." << std::endl;

      m_conn_handler->stop();
      m_conn_handler.reset();

      for (auto& client : m_clients) {
        client->stop();
      }
      m_clients.clear();

      std::cout << "Stopped Server!" << std::endl;
    }
};

int main(int argc, char** argv) {

  std::string stop;

  Server server;
  server.run();
  std::cin >> stop;
  server.stop();

  return 0;
}
