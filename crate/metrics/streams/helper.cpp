#include "helper.hpp"
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace metrics {
namespace streams {

helper_c::helper_c(const std::string& address, uint32_t port){
   _http_client = new httplib::Client(address, port); 
}

helper_c::~helper_c() {
   if (_http_client) {
      delete _http_client;
   }
}

helper_c::result helper_c::register_as_metric_stream_receiver(const std::string& address, uint32_t port) {

   std::string path = "/metric/stream/add/" + address + "/" + std::to_string(port);
   return issue_command(path);
}

helper_c::result helper_c::deregister_as_metric_stream_receiver(const std::string& address, uint32_t port) {

   std::string path = "/metric/stream/delete/" + address + "/" + std::to_string(port);
   return issue_command(path);
}

helper_c::result helper_c::issue_command(const std::string& path) {
   
   auto result = _http_client->Get(path);
   
   if (!result || (result.error() != httplib::Error::Success)) {
      return result::BAD_REQUEST;
   }

   json::jobject json_result;
   if (!json::jobject::tryparse(result->body.c_str(), json_result)) {
      return result::FAILED_TO_PARSE_RESPONSE;
   }

   if (json_result.has_key("status")) {

      if (json_result["status"].as_string() == "400") {
         return result::BAD_REQUEST;
      }

      if (json_result["status"].as_string() == "500") {
         return result::INTERNAL_SERVER_ERROR;
      }

      if (json_result["status"].as_string() != "200") {
         return result::UNKNOWN_ERROR;
      }

      if (json_result.has_key("data")) {
         if (json_result["data"].as_string() != "success") {
            return result::UNKNOWN_ERROR;
         } else {

            // All good :)
            return result::SUCCESS;
         }
      }
      
   }

   return result::UNKNOWN_ERROR;
}
   
} // namespace stream
} // namespace metrics
} // namespace crate