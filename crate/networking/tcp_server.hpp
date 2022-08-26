#ifndef CRATE_NETWORKING_TCP_SERVER_HPP
#define CRATE_NETWORKING_TCP_SERVER_HPP

#include "receiver_if.hpp"
#include <nettle/HostPort.hpp>
#include <nettle/TcpServer.hpp>

namespace crate {
namespace networking {

class tcp_handler;
class tcp_server {
public:

   tcp_server(const std::string& address, uint32_t port, receiver_if* receiver);
   ~tcp_server();
   bool start();
   void stop();
private:
   nettle::HostPort _host_port;
   receiver_if* _receiver {nullptr};
   nettle::TcpServer* _nettle_server {nullptr};
   tcp_handler* _handler {nullptr};
   void handle(std::string data);
};

}  // namespace networking
}  // namespace crate

#endif