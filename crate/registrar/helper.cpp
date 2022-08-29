#include "helper.hpp"

#include <iostream>

namespace crate {
namespace registrar {

helper_c::helper_c(const std::string& address, uint32_t port){
   _http_client = new httplib::Client(address, port); 
}

helper_c::~helper_c() {
   if (_http_client) {
      delete _http_client;
   }
}

helper_c::result helper_c::submit(node_v1_c& node) {

   std::string encoded_node;
   if (!node.encode_to(encoded_node)) {
      return result::INVALID_NODE_DATA;
   }
   
   auto [id, desc, sensors] = node.get_data();

   std::string submission = "/registrar/node/add/" + id + "/" + encoded_node;
   return issue_command(submission);
}

helper_c::result helper_c::remove(const std::string& id) {

   std::string submission = "/registrar/node/delete/" + id;
   return issue_command(submission);
}

helper_c::result helper_c::retrieve(const std::string& id, node_v1_c& node) {
   
   auto result = _http_client->Get("/registrar/node/fetch/" + id);

   if (!result || (result.error() != httplib::Error::Success)) {
      return result::UNABLE_TO_REACH_REGISTRAR;
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
         return result::REGISTRAR_INTERNAL_ERROR;
      }

      if (json_result["status"].as_string() != "200") {
         return result::BAD_FETCH;
      }

      if (json_result.has_key("data")) {
         if (json_result["data"].as_string() == "not found") {
            return result::NOT_FOUND;
         }
      } else {
         return result::BAD_FETCH;
      }
   }

   node.clear();
   if (!node.decode_from(json_result)) {
      return result::FAILURE;
   }

   return result::SUCCESS;
}

helper_c::result helper_c::issue_command(const std::string& path) {
   
   if (path.size() >= HTTP_URL_MAX_LEN) {
      return result::REQUEST_EXCEEDS_URL_MAX_LENGTH;
   }

   auto result = _http_client->Get(path);

   if (!result || (result.error() != httplib::Error::Success)) {
      return result::UNABLE_TO_REACH_REGISTRAR;
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
         return result::REGISTRAR_INTERNAL_ERROR;
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

} // namespace registrar
} // namespace crate