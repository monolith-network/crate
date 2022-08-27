#include "message_server.hpp"
#include <chrono>
#include <crate/externals/aixlog/logger.hpp>
#include <thread>

namespace crate {
namespace networking {

using namespace std::chrono_literals;

namespace {
   
static std::chrono::system_clock::time_point last_error_write = std::chrono::system_clock::now();

//    Receive a socket error from nettle. Use a timer to ensure that we don't spam the logger
//    instead, we limit the errors to 1 per second
//
void nettle_socket_error_cb(nettle::SocketError error) {
   std::chrono::duration<double, std::milli> error_write_delta = 
      std::chrono::high_resolution_clock::now() - last_error_write;

   if (error_write_delta.count() < 1000) {
      return;
   }

   switch (error)
   {
      case nettle::SocketError::SET_SOCK_OPT_RECV_TO:      LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SET_SOCK_OPT_RECV_TO"      << std::endl; break;
      case nettle::SocketError::SET_SOCK_OPT_SEND_TO:      LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SET_SOCK_OPT_SEND_TO"      << std::endl; break; 
      case nettle::SocketError::SOCKET_WRITE:              LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SOCKET_WRITE"              << std::endl; break;
      case nettle::SocketError::ATTEMPT_INIT_SETUP_SOCKET: LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "ATTEMPT_INIT_SETUP_SOCKET" << std::endl; break;
      case nettle::SocketError::SOCKET_CREATE:             LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SOCKET_CREATE"             << std::endl; break;
      case nettle::SocketError::SOCKET_BIND:               LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SOCKET_BIND"               << std::endl; break;
      case nettle::SocketError::SOCKET_LISTEN:             LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SOCKET_LISTEN"             << std::endl; break;
      case nettle::SocketError::WSAStartup:                LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "WSAStartup"                << std::endl; break;
      case nettle::SocketError::SOCKET_REUSEADDR:          LOG(TRACE) << TAG("crate::message_server::nettle_socket_error_cb") << "SOCKET_REUSEADDR"          << std::endl; break;
      default: break;
   }

   last_error_write = std::chrono::system_clock::now();
}

} // namespace anonymous

class tcp_handler : public nettle::TcpConnectionHandler {
public:
   tcp_handler(std::function<void(std::string message)> data_handler) 
               : _data_handler_cb(data_handler) {}

   void serverStarted() override {}
   void serverStopping() override {}
   void serverStopped() override {}

   void newConnection(nettle::Socket connection) override {

      // Read in 4 bytes to see the size of the message coming in
      //
      uint8_t buffer[4];
      connection.socketReadIn(buffer, 4 * sizeof(buffer[0]));

      uint32_t incoming_size = static_cast<uint32_t>(buffer[3]) << 24;
      incoming_size |= static_cast<uint32_t>(buffer[2]) << 16;
      incoming_size |= static_cast<uint32_t>(buffer[1]) << 8;
      incoming_size |= static_cast<uint32_t>(buffer[0]) << 0;

      // Read in that number of bytes
      //
      char* data_buffer = new char[incoming_size];
      connection.socketReadIn(data_buffer, incoming_size);

      _data_handler_cb(std::string(data_buffer));
      delete []data_buffer;
   }

private:
   std::function<void(std::string message)> _data_handler_cb;
};

message_server::message_server(const std::string& address, uint32_t port, message_receiver_if* receiver) 
   : _host_port(address, port),
      _receiver(receiver) {

}

message_server::~message_server() {

}

bool message_server::start() {
   if (!_nettle_server) {
      if (!_handler) {
         _handler = new tcp_handler(
               std::bind(&message_server::handle, this, std::placeholders::_1)
         );
      }
      _nettle_server = new nettle::TcpServer(
         _host_port,
         *_handler,
         nettle_socket_error_cb,
         10,
         0
      );
   }

   if (!_nettle_server->serve()) {
      LOG(WARNING) << TAG("crate::message_server::start") 
                     << "Unable to start nettle tcp server\n";
      return false;
   }
   return true;
}

void message_server::stop() {
   if (_nettle_server) {
      _nettle_server->stop();
      if (_handler) {
         delete _handler;
         _handler = nullptr;
      }
      delete _nettle_server;
      _nettle_server = nullptr;
   }
}

void message_server::handle(std::string data) {

   if (!_receiver) {
      LOG(ERROR) << TAG("message_server::handle") << "Receiver interface was not set\n";
      return;
   }

   // Since the receiver doesn't care to talk back to the client
   // we will launch an async thread so receiver processing doesn't
   // hold up a connection
   std::thread {
      [](message_receiver_if* receiver, std::string data) {
         receiver->receive_message(data);
      },
      _receiver,
      data
   }.detach();
}


}  // namespace networking
}  // namespace crate