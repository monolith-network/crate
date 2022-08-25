#include "node_v1.hpp"

namespace crate {
namespace registrar {

node_v1::node_v1(){}

node_v1::node_v1(const std::string& id) : _id(id) {}

bool node_v1::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool node_v1::decode_from(json::jobject json_object) {

   if (!json_object.has_key("id")) {
      return false;
   }

   if (!json_object.has_key("description")) {
      return false;
   }

   if (!json_object.has_key("sensors")) {
      return false;
   }

   auto suspected_sensor_list = json_object["sensors"].as_object();
   if (!suspected_sensor_list.is_array()) {
      return false;
   }

   _id = json_object["id"].as_string();
   _description = json_object["description"].as_string();

   if (suspected_sensor_list.size() == 0) {
      check_validity();
      return !_invalid;
   }

   for(size_t i = 0; i < suspected_sensor_list.size(); i++) {
      json::jobject entry = suspected_sensor_list.array(i);
   
      if (!entry.has_key("id")) {
         return false;
      }
      if (!entry.has_key("type")) {
         return false;
      }
      if (!entry.has_key("description")) {
         return false;
      }

      // Decode the data and attempt to add it to the sensor list
      if (!add_sensor({
         .id = entry["id"].as_string(),
         .type = entry["type"].as_string(),
         .description = entry["description"].as_string(),
      })) {
         return false;
      }
   }

   check_validity();
   return !_invalid;
}

bool node_v1::encode_to(std::string& output_string) {

   check_validity();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   std::string s = "{\"id\":\"" + _id + 
                   "\",\"description\":\"" + _description +
                   "\",\"sensors\":[";

   if (_sensor_list.empty()) {
      s += "]}";
      output_string = s;
      return true;   
   }

   for(auto &e : _sensor_list) {
      s += "{\"id\":\"" + e.id + 
            "\",\"type\":\"" + e.type + 
            "\",\"description\":\"" + e.description + 
            "\"},"; 
   }

   // Remove the last comma
   s.pop_back();
   s += "]}";
   output_string = s;
   return true;   
}

void node_v1::set_id(const std::string& id) {
   _id = id;
}

void node_v1::set_description(const std::string& description) {
   _description = description;
}

bool node_v1::add_sensor(node_v1::sensor new_sensor) {
   if (new_sensor.id.empty()) {
      return false;
   }

   for(auto& s : _sensor_list) {
      if (s.id == new_sensor.id) {
         return false;
      }
   }

   _sensor_list.push_back(new_sensor);
   return true;
}

void node_v1::check_validity() {
   if (_id.empty()) {
      _invalid = true;
      return;
   }
   _invalid = false;
}

std::tuple<
   std::string, 
   std::string, 
   std::vector<node_v1::sensor> 
      > node_v1::get_data() const {
   return {_id, _description, _sensor_list};
}

void node_v1::clear() {
   _invalid = true;
   _id.clear();
   _description.clear();
   _sensor_list.clear();
}

} // namespace registrar
} // namespace crate