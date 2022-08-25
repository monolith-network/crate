#include "retriever.hpp"

#include <httplib.h>

namespace crate {
namespace registrar {

retriever::retriever(const std::string& registrar_address, short registrar_port)
   : _registrar_address(registrar_address),
      _registrar_port(registrar_port) {

}

retriever::result retriever::retrieve(const std::string& id, node_v1& node) {
   
   httplib::Client cli(_registrar_address, _registrar_port);
   
   auto result = cli.Get("/fetch/" + id);

   if (!result || (result.error() != httplib::Error::Success)) {
      return result::UNABLE_TO_REACH_REGISTRAR;
   }

   json::jobject json_result;
   if (!json::jobject::tryparse(result->body.c_str(), json_result)) {
      return result::BAD_FETCH;
   }

   if (json_result.has_key("status")) {

      if (json_result["status"].as_string() == "400") {
         return result::RETRIEVER_INTERNAL_ERROR;
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
      return result::FAILED_DECODE;
   }

   return result::SUCCESS;
}

} // namespace registrar
} // namespace crate
