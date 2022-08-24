#include "cache.hpp"
#include <algorithm>
#include <functional>
#include <httplib.h>
#include <crate/registrar/entry/node_v1.hpp>
#include <crate/externals/aixlog/logger.hpp>
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace registrar {

namespace {
   // Calculate the has of two strings together
   std::size_t calculate_hash(const std::string& s1, const std::string& s2) {
      std::size_t h1 = std::hash<std::string>{}(s1);
      std::size_t h2 = std::hash<std::string>{}(s2);
      return h1 ^ (h2 << 1);
   }
}

cache::cache(const std::string& registrar_address, short registrar_port)
   : _registrar_address(registrar_address),
      _registrar_port(registrar_port) {
}

void cache::prune() {

   // TODO: DO THIS NEXT


   // Take a time stamp
   // Lock the mutex
   // Iterate over all items in _cache_items
   // If delta from stamp - entry.retrieved is > _prune_time 
   //    delete from _primary cache using entry.hash, 
   //    then remove from _cache_items

}

double cache::get_prune_time() const {
   return _prune_time;
}

cache::result cache::check_cache_for_node_sensor(const std::string& node,
                                                 const std::string& sensor) {

   auto hashed_item = calculate_hash(node, sensor);

   // Check the primary cache for hashed item
   {
      const std::lock_guard<std::mutex> lock(_mutex);
      if (_primary_cache.contains(hashed_item)) {
         return result::KNOWN;
      }
   }

   return check_registrar(hashed_item, node, sensor);
}

cache::result cache::check_registrar(const size_t hash, 
                              const std::string& node, 
                              const std::string& sensor) {

   // Check if item exists in registrar
   // If it does, return 

   httplib::Client cli(_registrar_address, _registrar_port);
   
   auto result = cli.Get("/fetch/" + node);

   if (!result || (result.error() != httplib::Error::Success)) {
      return result::UNABLE_TO_REACH_REGISTRAR;
   }

   json::jobject json_result;
   if (!json::jobject::tryparse(result->body.c_str(), json_result)) {
      return result::BAD_FETCH;
   }

   if (json_result.has_key("status")) {

      if (json_result["status"].as_object() == 400) {
         return result::CACHE_INTERNAL_ERROR;
      }

      if (json_result["status"].as_object() == 500) {
         return result::REGISTRAR_INTERNAL_ERROR;
      }

      if (json_result["status"].as_object() != 200) {
         return result::BAD_FETCH;
      }

      if (json_result.has_key("data")) {
         if (json_result["data"].as_string() == "not found") {
            return result::UNKNOWN_NODE;
         }
      } else {
         return result::BAD_FETCH;
      }
   }

   node_v1 decoded_node;
   if (!decoded_node.decode_from(json_result)) {
      return result::BAD_FETCH;
   }

   auto [id, desc, sensors] = decoded_node.get_data();
   for (auto& s : sensors) {
      if (s.id == sensor) {

         // We found the match, so plop it int he caches
         {
            const std::lock_guard<std::mutex> lock(_mutex);

            _cache_items.emplace_back(cache_entry {
               .hash = hash,
               .retrieved = std::chrono::system_clock::now()
            });
            _primary_cache.insert(hash);
         }
         return result::KNOWN;
      }
   }

   // If we get here we have the node, but we did not locate the sensor
   return result::UNKNOWN_NODE_SENSOR;
}

} // namespace registrar
} // namespace crate