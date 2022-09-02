#include "reading_v1.hpp"
#include <chrono>
#include <sstream>
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace metrics {

sensor_reading_v1_c::sensor_reading_v1_c() {

}

sensor_reading_v1_c::sensor_reading_v1_c(int64_t timestamp, 
                                       std::string id, 
                                       std::string sensor_id, 
                                       double value) 
                                    : _timestamp(timestamp),
                                       _node_id(id),
                                       _sensor_id(sensor_id),
                                       _value(value) {
}

void sensor_reading_v1_c::stamp()
{
   _timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count();
}

bool sensor_reading_v1_c::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool sensor_reading_v1_c::decode_from(json::jobject json_object) {

   if (!json_object.has_key("timestamp")) {
      return false;
   }

   if (!json_object.has_key("node_id")) {
      return false;
   }

   if (!json_object.has_key("sensor_id")) {
      return false;
   }

   if (!json_object.has_key("value")) {
      return false;
   }

   std::stringstream ts_ss(json_object["timestamp"].as_string());
   ts_ss >> _timestamp;

   std::stringstream v_ss(json_object["value"].as_string());
   v_ss >> _value;

   _node_id = json_object["node_id"].as_string();
   _sensor_id = json_object["sensor_id"].as_string();

   check_valid();
   return !_invalid;
}

bool sensor_reading_v1_c::encode_to(std::string& output_string) {

   check_valid();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   output_string = "{\"timestamp\":" + std::to_string(_timestamp) +
                     ",\"node_id\":\"" + _node_id + "\"," +
                     "\"sensor_id\":\"" + _sensor_id + "\"," + 
                     "\"value\":" + std::to_string(_value) + 
                     "}";

   return true;
}

void sensor_reading_v1_c::check_valid() {
   if (_node_id.empty() || 
         _sensor_id.empty() ||
         _timestamp == 0) {
      _invalid = true;
   } else {
      _invalid = false;
   }
}

std::tuple<int64_t, std::string, std::string, double> sensor_reading_v1_c::get_data() {
   return {_timestamp, _node_id, _sensor_id, _value};
}
   
} // namespace metrics
} // namespace crate