#include "controller_v1.hpp"
#include <sstream>

namespace crate {
namespace registrar {

controller_v1_c::controller_v1_c(){}

controller_v1_c::controller_v1_c(const std::string& id) : _id(id) {}

controller_v1_c::controller_v1_c(const std::string& id,
                   const std::string& description,
                   const std::string& address, 
                   uint32_t port) 
                   : _id(id), _description(description), _ip(address), _port(port) {}

bool controller_v1_c::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool controller_v1_c::decode_from(json::jobject json_object) {

   if (!json_object.has_key("id")) {
      return false;
   }

   if (!json_object.has_key("description")) {
      return false;
   }

   if (!json_object.has_key("ip")) {
      return false;
   }

   if (!json_object.has_key("port")) {
      return false;
   }

   if (!json_object.has_key("actions")) {
      return false;
   }

   auto suspected_action_list = json_object["actions"].as_object();
   if (!suspected_action_list.is_array()) {
      return false;
   }

   _id = json_object["id"].as_string();
   _description = json_object["description"].as_string();

   _ip = json_object["ip"].as_string();
   std::stringstream ts_ss(json_object["port"].as_string());
   ts_ss >> _port;
   _description = json_object["description"].as_string();

   if (suspected_action_list.size() == 0) {
      check_validity();
      return !_invalid;
   }

   for(size_t i = 0; i < suspected_action_list.size(); i++) {
      json::jobject entry = suspected_action_list.array(i);
   
      if (!entry.has_key("id")) {
         return false;
      }
      if (!entry.has_key("description")) {
         return false;
      }

      // Decode the data and attempt to add it to the action list
      if (!add_action({
         .id = entry["id"].as_string(),
         .description = entry["description"].as_string(),
      })) {
         return false;
      }
   }

   check_validity();
   return !_invalid;
}

bool controller_v1_c::encode_to(std::string& output_string) {

   check_validity();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   std::string s = "{\"id\":\"" + _id + 
                   "\",\"ip\":\"" + _ip + 
                   "\",\"port\":" + std::to_string(_port) + 
                   ",\"description\":\"" + _description +
                   "\",\"actions\":[";

   if (_action_list.empty()) {
      s += "]}";
      output_string = s;
      return true;   
   }

   for(auto &e : _action_list) {
      s += "{\"id\":\"" + e.id + 
            "\",\"description\":\"" + e.description + 
            "\"},"; 
   }

   // Remove the last comma
   s.pop_back();
   s += "]}";
   output_string = s;
   return true;   
}

void controller_v1_c::set_id(const std::string& id) {
   _id = id;
}

void controller_v1_c::set_description(const std::string& description) {
   _description = description;
}

void controller_v1_c::set_ip(const std::string& ip) {
   _ip = ip;
}

void controller_v1_c::set_port(uint32_t port) {
   _port = port;
}

bool controller_v1_c::add_action(controller_v1_c::action_s new_action) {
   if (new_action.id.empty()) {
      return false;
   }

   for(auto& s : _action_list) {
      if (s.id == new_action.id) {
         return false;
      }
   }

   _action_list.push_back(new_action);
   return true;
}

void controller_v1_c::check_validity() {
   if (_id.empty()) {
      _invalid = true;
      return;
   }
   _invalid = false;
}

std::tuple<
   std::string, 
   std::string, 
   std::string,
   uint32_t,
   std::vector<controller_v1_c::action_s> 
      > controller_v1_c::get_data() const {
   return {_id, _description, _ip, _port, _action_list};
}

void controller_v1_c::clear() {
   _invalid = true;
   _id.clear();
   _description.clear();
   _ip.clear();
   _port = 0;
   _action_list.clear();
}

} // namespace registrar
} // namespace crate