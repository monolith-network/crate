#include "cache.hpp"

#include <crate/externals/aixlog/logger.hpp>

namespace crate {
namespace registrar {

cache::cache(const std::string& registrar_address, short registrar_port) {
   _registrar_endpoint = registrar_address + 
                         ":" + 
                         std::to_string(registrar_port);
}

cache::result cache::check_cache_for_node_sensor(const std::string& node,
                                                 const std::string& sensor) {

   // Lock mutex

   // Check map for existence of node

      // If exists, check for sensor

         // If node and sensor good, return KNOWN

         // If sensor no exist, return UNKNOWN_NODE_SENSOR

      // If node not exist free the mutex
      // - Use httplib to check if a node exists
      //   - If it exist, get its data
      //   - Re-mutex the map
      //   - store the data in the map
      //   - return KNOWN

      // - If registrar doesn't know about node
      //    return UNKNOWN_NODE

      // If we can't reach the registrar, return UNABLE_TO_REACH_REGISTRAR


   // Check for node / sensor using httplib

   return result::UNABLE_TO_REACH_REGISTRAR;
}

} // namespace registrar
} // namespace crate