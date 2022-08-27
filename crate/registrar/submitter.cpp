#include "submitter.hpp"

#include <httplib.h>

#include <iostream>

namespace crate {
namespace registrar {

submitter::submitter(const std::string& registrar_address, short registrar_port)
   : _registrar_address(registrar_address),
      _registrar_port(registrar_port) {

}

submitter::result submitter::submit(node_v1& node) {

   std::string encoded_node;
   if (!node.encode_to(encoded_node)) {
      return result::INVALID_NODE_DATA;
   }

   httplib::Client cli(_registrar_address, _registrar_port);
   
   auto [id, desc, sensors] = node.get_data();

   std::string submission = "/submit/" + id + "/" + encoded_node;

   if (submission.size() >= HTTP_URL_MAX_LEN) {
      return result::REQUEST_EXCEEDS_URL_MAX_LENGTH;
   }

   auto result = cli.Get(submission);

   if (!result || (result.error() != httplib::Error::Success)) {
      return result::UNABLE_TO_REACH_REGISTRAR;
   }

   json::jobject json_result;
   if (!json::jobject::tryparse(result->body.c_str(), json_result)) {
      return result::BAD_FETCH;
   }

   if (json_result.has_key("status")) {

      if (json_result["status"].as_string() == "400") {
         return result::SUBMIT_INTERNAL_ERROR;
      }

      if (json_result["status"].as_string() == "500") {
         return result::REGISTRAR_INTERNAL_ERROR;
      }

      if (json_result["status"].as_string() != "200") {
         return result::BAD_FETCH;
      }

      if (json_result.has_key("data")) {
         if (json_result["data"].as_string() != "success") {
            return result::SUBMISSION_FAILURE;
         } else {
            return result::SUCCESS;
         }
      }
   }
   return result::BAD_FETCH;
}

} // namespace registrar
} // namespace crate