#ifndef CRATE_NETWORKING_MESSAGE_WRITER_HPP
#define CRATE_NETWORKING_MESSAGE_WRITER_HPP

#include <nettle/Writer.hpp>
#include <string>

namespace crate {
namespace networking {

//! \brief Write message to a message server
class message_writer_c
{
public:
   //! \brief Create a writer
   //! \param address The IPV4 Address to write to
   //! \param port The port to write to
   message_writer_c(const std::string& address, uint32_t port);

   //! \brief Change the destination that the writer will write to
   //! \param address The IPV4 Address to write to
   //! \param port The port to write to
   void change_destination(const std::string& address, uint32_t port);

   //! \brief Attempt to write data to the destination
   //! \param[in] data The data to write out to the destination
   //! \param[out] okay Indicate if the write failed to bind
   //! \returns Number of bytes sent to destination
   int write(std::string data, bool& okay);

private:
   nettle::HostPort _host_port;
};

}  // namespace networking
}  // namespace crate

#endif