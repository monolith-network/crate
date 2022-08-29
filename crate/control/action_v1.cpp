#include "action_v1.hpp"
#include <chrono>
#include <sstream>
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace metrics {

controller_action_v1_c::controller_action_v1_c() {

}

controller_action_v1_c::controller_action_v1_c(int64_t timestamp, 
                                       std::string id, 
                                       std::string action_id, 
                                       double value) 
                                    : _timestamp(timestamp),
                                       _controller_id(id),
                                       _action_id(action_id),
                                       _value(value) {
}

void controller_action_v1_c::stamp()
{
   _timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count();
}

bool controller_action_v1_c::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool controller_action_v1_c::decode_from(json::jobject json_object) {

   if (!json_object.has_key("timestamp")) {
      return false;
   }

   if (!json_object.has_key("controller_id")) {
      return false;
   }

   if (!json_object.has_key("action_id")) {
      return false;
   }

   if (!json_object.has_key("value")) {
      return false;
   }

   std::stringstream ts_ss(json_object["timestamp"].as_string());
   ts_ss >> _timestamp;

   std::stringstream v_ss(json_object["value"].as_string());
   v_ss >> _value;

   _controller_id = json_object["controller_id"].as_string();
   _action_id = json_object["action_id"].as_string();

   check_valid();
   return !_invalid;
}

bool controller_action_v1_c::encode_to(std::string& output_string) {

   check_valid();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   output_string = "{\"timestamp\":" + std::to_string(_timestamp) +
                     ",\"controller_id\":\"" + _controller_id + "\"," +
                     "\"action_id\":\"" + _action_id + "\"," + 
                     "\"value\":" + std::to_string(_value) + 
                     "}";

   return true;
}

void controller_action_v1_c::check_valid() {
   if (_controller_id.empty() || 
         _action_id.empty() ||
         _timestamp == 0) {
      _invalid = true;
   } else {
      _invalid = false;
   }
}

std::tuple<int64_t, std::string, std::string, double> controller_action_v1_c::getData() {
   return {_timestamp, _controller_id, _action_id, _value};
}
   
} // namespace metrics
} // namespace crate