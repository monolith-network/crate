#include "version_v1.hpp"
#include <chrono>
#include <sstream>
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace app {

version_v1_c::version_v1_c() {

}

version_v1_c::version_v1_c(std::string name, std::string hash, semver_s semver)
   : _name(name), _hash(hash), _semver(semver) {
}

bool version_v1_c::decode_from(const std::string& json_data) {

   json::jobject json_result;
   if (!json::jobject::tryparse(json_data.c_str(), json_result)) {
      return false;
   }
   return decode_from(json_result);
}

bool version_v1_c::decode_from(json::jobject json_object) {

   if (!json_object.has_key("name")) {
      return false;
   }

   if (!json_object.has_key("hash")) {
      return false;
   }

   if (!json_object.has_key("version_major")) {
      return false;
   }

   if (!json_object.has_key("version_minor")) {
      return false;
   }

   if (!json_object.has_key("version_patch")) {
      return false;
   }

   _name = json_object["name"].as_string();
   _hash = json_object["hash"].as_string();
   _semver.major =  json_object["version_major"].as_string();
   _semver.minor =  json_object["version_minor"].as_string();
   _semver.patch =  json_object["version_patch"].as_string();

   check_valid();
   return !_invalid;
}

bool version_v1_c::encode_to(std::string& output_string) {

   check_valid();
   if (_invalid) {
      return false;
   }

   output_string.clear();
   output_string = "{\"name\":\"" + _name + "\"" +
                     ",\"hash\":\"" + _hash + "\"," +
                     "\"version_major\":\"" + _semver.major + "\"," + 
                     "\"version_minor\":\"" + _semver.minor + "\"," + 
                     "\"version_patch\":\"" + _semver.patch + "\"," + 
                     "}";

   return true;
}

void version_v1_c::check_valid() {
   if (_name.empty() || 
         _hash.empty() ||
         _semver.major.empty() ||
         _semver.minor.empty() ||
         _semver.patch.empty()) {
      _invalid = true;
   } else {
      _invalid = false;
   }
}

std::tuple<std::string, std::string, semver_s> version_v1_c::get_data() {
   return {_name, _hash, _semver};
}
   
} // namespace metrics
} // namespace crate