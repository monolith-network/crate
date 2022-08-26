#include "tcp_server.hpp"
#include <chrono>
#include <crate/externals/aixlog/logger.hpp>

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
      case nettle::SocketError::SET_SOCK_OPT_RECV_TO:      LOG(TRACE) << TAG("nettle_socket_error_cb") << "SET_SOCK_OPT_RECV_TO"      << std::endl; break;
      case nettle::SocketError::SET_SOCK_OPT_SEND_TO:      LOG(TRACE) << TAG("nettle_socket_error_cb") << "SET_SOCK_OPT_SEND_TO"      << std::endl; break; 
      case nettle::SocketError::SOCKET_WRITE:              LOG(TRACE) << TAG("nettle_socket_error_cb") << "SOCKET_WRITE"              << std::endl; break;
      case nettle::SocketError::ATTEMPT_INIT_SETUP_SOCKET: LOG(TRACE) << TAG("nettle_socket_error_cb") << "ATTEMPT_INIT_SETUP_SOCKET" << std::endl; break;
      case nettle::SocketError::SOCKET_CREATE:             LOG(TRACE) << TAG("nettle_socket_error_cb") << "SOCKET_CREATE"             << std::endl; break;
      case nettle::SocketError::SOCKET_BIND:               LOG(TRACE) << TAG("nettle_socket_error_cb") << "SOCKET_BIND"               << std::endl; break;
      case nettle::SocketError::SOCKET_LISTEN:             LOG(TRACE) << TAG("nettle_socket_error_cb") << "SOCKET_LISTEN"             << std::endl; break;
      case nettle::SocketError::WSAStartup:                LOG(TRACE) << TAG("nettle_socket_error_cb") << "WSAStartup"                << std::endl; break;
      case nettle::SocketError::SOCKET_REUSEADDR:          LOG(TRACE) << TAG("nettle_socket_error_cb") << "SOCKET_REUSEADDR"          << std::endl; break;
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

   // Handle parsing and routing network message with async
   //
   void async_cb(char* data_buffer, uint32_t size) {
      std::string data_json(data_buffer, size);
      _data_handler_cb(data_json);
   }

   void newConnection(nettle::Socket& connection) override {

      // Read in 4 bytes to see the size of the message coming in
      //
      uint8_t buffer[4];
      connection.socketReadIn(buffer, 4 * sizeof(buffer[0]));

      uint32_t incoming_size = 
         buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);

      // Read in that number of bytes
      //
      char* data_buffer = new char[incoming_size];
      auto read_in = connection.socketReadIn(data_buffer, incoming_size);
      connection.socketClose();

      _data_handler_cb(std::string(data_buffer));

      delete []data_buffer;
   }

private:
   std::function<void(std::string message)> _data_handler_cb;
};

tcp_server::tcp_server(const std::string& address, uint32_t port, receiver_if* receiver) 
   : _host_port(address, port),
      _receiver(receiver) {

}

tcp_server::~tcp_server() {

}



}  // namespace networking
}  // namespace crate