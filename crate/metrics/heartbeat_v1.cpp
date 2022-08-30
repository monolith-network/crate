#include "heartbeat_v1.hpp"
#include <chrono>
#include <sstream>
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace metrics {

heartbeat_v1_c::heartbeat_v1_c() {

}

heartbeat_v1_c::heartbeat_v1_c(std::string id) 
                                    : _id(id) {
}

bool heartbeat_v1_c::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool heartbeat_v1_c::decode_from(json::jobject json_object) {

   if (!json_object.has_key("heartbeat")) {
      return false;
   }

   _id = json_object["heartbeat"].as_string();

   check_valid();
   return !_invalid;
}

bool heartbeat_v1_c::encode_to(std::string& output_string) {

   check_valid();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   output_string = "{\"heartbeat\":\"" + _id + "\"}";

   return true;
}

void heartbeat_v1_c::check_valid() {
   if (_id.empty()) {
      _invalid = true;
   } else {
      _invalid = false;
   }
}

std::string heartbeat_v1_c::getData() {
   return _id;
}
   
} // namespace metrics
} // namespace crate