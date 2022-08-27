#include "message_writer.hpp"

namespace crate {
namespace networking {

message_writer::message_writer(const std::string& address, uint32_t port) 
   : _host_port(address, port) {}

void message_writer::change_destination(const std::string& address, uint32_t port) {
   _host_port = nettle::HostPort(address, port);
}

int message_writer::write(std::string data, bool& okay) {
   okay = false;

   nettle::Writer writer(
         _host_port,
         nettle::WriterType::TCP);

   if(writer.hasError()) {
      writer.socketClose();
      return false;
   }

   uint32_t data_len = data.length();

   uint8_t data_len_buffer[4];
   data_len_buffer[0] = data_len >> 0;
   data_len_buffer[1] = data_len >> 8;
   data_len_buffer[2] = data_len >> 16;
   data_len_buffer[3] = data_len >> 24;

   // Write out expected length
   writer.socketWriteOut(data_len_buffer, 4 * sizeof(data_len_buffer[0]));

   // Write out data
   int data_written = writer.socketWriteOut(data.c_str(), data.length());

   // Close endpoint
   writer.socketClose();

   okay = true;

   return data_written;
}
   
}  // namespace networking
}  // namespace crate
