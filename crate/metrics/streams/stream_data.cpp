#include "stream_data.hpp"
#include <chrono>
#include <sstream>
#include <crate/externals/simplejson/json.hpp>
#include <crate/metrics/reading.hpp>

namespace crate {
namespace metrics {
namespace streams {

stream_data_v1::stream_data_v1(uint64_t sequence) : _timestamp(0), _sequence(sequence) {}

stream_data_v1::stream_data_v1(uint64_t sequence,
                               std::vector<crate::metrics::sensor_reading_v1> metrics) 
                               : _timestamp(0), 
                                 _sequence(sequence),
                                 _data(metrics) {}

void stream_data_v1::stamp() {
   _timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count();
}

void stream_data_v1::add_metric(crate::metrics::sensor_reading_v1 metric) {
   _data.push_back(metric);
}

bool stream_data_v1::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool stream_data_v1::decode_from(json::jobject json_object) {
   if (!json_object.has_key("timestamp")) {
      return false;
   }

   if (!json_object.has_key("sequence")) {
      return false;
   }

   if (!json_object.has_key("data")) {
      return false;
   }

   json::jobject data_array;
   if (!json::jobject::tryparse(json_object.get("data").c_str(), data_array)) {
      return false;
   }
   
   if (!data_array.is_array()) {
      return false;
   }

   for(size_t i = 0; i < data_array.size(); i++) {
      json::jobject entry = data_array.array(i);

      crate::metrics::sensor_reading_v1 reading;

      if (!reading.decode_from(entry)) {
         return false;
      }

      _data.push_back(reading);
   }

   std::stringstream ts_ss(json_object["timestamp"].as_string());
   ts_ss >> _timestamp;

   std::stringstream s_ss(json_object["sequence"].as_string());
   s_ss >> _sequence;

   check_validity();
   return !_invalid;
}

bool stream_data_v1::encode_to(std::string& output_string) {

   check_validity();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   output_string = "{\"timestamp\":" + std::to_string(_timestamp) +
                   ",\"sequence\":" + std::to_string(_sequence) +
                   ",\"data\":[";

   for(auto& metric : _data) {
      std::string encoded;
      if (!metric.encode_to(encoded)) {
         return false;
      }
      output_string += encoded;
      output_string += ",";
   }

   output_string.pop_back();
   output_string += "]}";
   return true;
}

size_t stream_data_v1::size() const {
   return _data.size();
}

bool stream_data_v1::empty() const {
   return _data.empty();
}

std::tuple<int64_t, 
               uint64_t, 
               std::vector<crate::metrics::sensor_reading_v1>> stream_data_v1::get_data() {
   return {_timestamp, _sequence, _data};
}

void stream_data_v1::check_validity() {
   if (_timestamp == 0 || empty()) {
      _invalid = true;
   } else {
      _invalid = false;
   }
}
   
} // namespace stream
} // namespace metrics
} // namespace crate