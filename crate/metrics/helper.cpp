#include "helper.hpp"

namespace crate {
namespace metrics {


helper_c::helper_c(const endpoint_type_e type, const std::string& address, uint32_t port){

   switch (type) {
      case endpoint_type_e::HTTP: 
         _http_client = new httplib::Client(address, port); 
      break;
      case endpoint_type_e::TCP:  
         _tcp_writer  = new crate::networking::message_writer_c(address, port);
      break;
   }   
}

helper_c::~helper_c() {
   if (_http_client) {
      delete _http_client;
   }
   if (_tcp_writer) {
      delete _tcp_writer;
   }
}

helper_c::result helper_c::submit(heartbeat_v1_c& heartbeat) {

   std::string encoded;
   if (!heartbeat.encode_to(encoded)) {
      return result::INVALID_DATA;
   }

   if (_http_client) {
      std::string path = "/metric/heartbeat/" + encoded;
      return issue_command(path);
   } else if (_tcp_writer) {
      return write_tcp(encoded);
   }
   return result::INTERNAL_ERROR;
}

helper_c::result helper_c::submit(sensor_reading_v1_c& reading) {
   
   std::string encoded;
   if (!reading.encode_to(encoded)) {
      return result::INVALID_DATA;
   }
   
   if (_http_client) {
      std::string path = "/metric/submit/" + encoded;
      return issue_command(path);
   } else if (_tcp_writer) {
      return write_tcp(encoded);
   }
   return result::INTERNAL_ERROR;
}

helper_c::result helper_c::issue_command(const std::string& path) {
   
   if (path.size() >= HTTP_URL_MAX_LEN) {
      return result::REQUEST_EXCEEDS_URL_MAX_LENGTH;
   }

   auto result = _http_client->Get(path);

   if (!result || (result.error() != httplib::Error::Success)) {
      return result::UNABLE_TO_REACH;
   }

   json::jobject json_result;
   if (!json::jobject::tryparse(result->body.c_str(), json_result)) {
      return result::BAD_FETCH;
   }

   if (json_result.has_key("status")) {

      if (json_result["status"].as_string() == "400") {
         return result::INTERNAL_ERROR;
      }

      if (json_result["status"].as_string() == "500") {
         return result::MONOLITH_INTERNAL_ERROR;
      }

      if (json_result["status"].as_string() != "200") {
         return result::BAD_FETCH;
      }

      if (json_result.has_key("data")) {
         if (json_result["data"].as_string() != "success") {
            return result::FAILURE;
         } else {
            return result::SUCCESS;
         }
      }
   }
   return result::BAD_FETCH;
}

helper_c::result helper_c::write_tcp(const std::string& data) {
   bool okay {false};
   auto written = _tcp_writer->write(data, okay);

   if (written != data.size() || !okay) {
      return result::WRITE_FAILED;
   }
   return result::SUCCESS;
}

} // namespace metrics
} // namespace crate