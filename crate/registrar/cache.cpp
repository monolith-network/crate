#include "cache.hpp"
#include <algorithm>
#include <functional>
#include <crate/externals/aixlog/logger.hpp>
#include <httplib.h>

// TODO: Remove
#include <iostream> 



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

cache::result cache::check_cache_for_node_sensor(const std::string& node,
                                                 const std::string& sensor) {

   auto hashed_item = calculate_hash(node, sensor);

   std::cout << "Hash of " << node << " and sensor " << sensor << " is " << hashed_item << std::endl;

   // Check the primary cache for hashed item
   {
      const std::lock_guard<std::mutex> lock(_mutex);
      if (_primary_cache.contains(hashed_item)) {
         return result::KNOWN;
      }
   }

   return result::UNABLE_TO_REACH_REGISTRAR;
}

std::tuple<bool, cache::result> cache::check_registrar(const size_t hash, 
                              const std::string& node, 
                              const std::string& sensor) {

   // Check if item exists in registrar
   // If it does, return 

   httplib::Client cli(_registrar_address, _registrar_port);
   
   auto result = cli.Get("/fetc/" + node);

   // TODO pull the data, if the node exists, and the sensor is within the node add the 
   // hash to the _cache and create an entry in `_cache_items`
}

} // namespace registrar
} // namespace crate