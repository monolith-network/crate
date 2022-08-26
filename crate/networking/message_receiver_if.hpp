#ifndef CRATE_NETWORKING_MESSAGE_RECEIVER_IF_HPP
#define CRATE_NETWORKING_MESSAGE_RECEIVER_IF_HPP

#include <string>

/*
   About:
      A lot of server setups being used within this system just need to receive a message
      and don't need to do any correspondence past that. To make things standardized and 
      without lots of boilerplate, a service can spin up a crate server and give it this
      interface to receive data
*/

namespace crate {
namespace networking {

//! \brief Simple receiver interface to allow for the receiving of a single message
//!        from an external source.
class message_receiver_if {
public:
   //! \brief Receive the message
   //! \param message The message to receive
   virtual void receive_message(std::string message) = 0;
};

}  // namespace networking
}  // namespace crate

#endif