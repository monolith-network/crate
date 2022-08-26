#ifndef CRATE_NETWORKING_MESSAGE_SERVER_HPP
#define CRATE_NETWORKING_MESSAGE_SERVER_HPP

#include "message_receiver_if.hpp"
#include <nettle/HostPort.hpp>
#include <nettle/TcpServer.hpp>

namespace crate {
namespace networking {

class tcp_handler;
//! \brief A basic tcp server that when a message is received it forwards it on to the receiver interface
class message_server {
public:
   //! \brief Create the message server with and addres, port, and receiver interface
   message_server(const std::string& address, uint32_t port, message_receiver_if* receiver);
   ~message_server();

   //! \brief Start the server
   //! \returns true iff the server could start
   //! \post Any valid message coming through to the server will be passed to the receiver
   bool start();

   //! \brief Stop the server
   //! \post Messages will stop being received
   void stop();
private:
   nettle::HostPort _host_port;
   message_receiver_if* _receiver {nullptr};
   nettle::TcpServer* _nettle_server {nullptr};
   tcp_handler* _handler {nullptr};
   void handle(std::string data);
};

}  // namespace networking
}  // namespace crate

#endif