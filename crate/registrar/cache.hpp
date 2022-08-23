#ifndef CRATE_REGISTRAR_CACHE_HPP
#define CRATE_REGISTRAR_CACHE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace crate {
namespace registrar {

//! \brief A class that interfaces with and caches results from the
//!        node registrar application for the sake of determining if
//!        a node exists, and if so, if a given sensor is registered as 
//!        being part of it
class cache {
public:

   //! \brief The results that can come from the node cache
   enum class result {
      KNOWN,
      UNKNOWN_NODE,
      UNKNOWN_NODE_SENSOR,
      UNABLE_TO_REACH_REGISTRAR
   };

   cache() = delete;

   //! \brief Create the node cache
   //! \param registrar_address The address of the registrar database
   //! \param registrar_port The port of the registrar
   cache(const std::string& registrar_address, short registrar_port);

   //! \brief Check if a node exists and if a specific sensor belongs to that node
   //! \param node The node that we wan't to check if it exists
   //! \param sensor The name of the sensor to check belongs to the node
   //! \returns A result enumeration specifying if everything is known, or if not, 
   //!          what isn't known
   result check_cache_for_node_sensor(const std::string& node, const std::string& sensor);

private:
   std::string _registrar_endpoint;
   std::unordered_map<std::string, std::vector<std::string>> _cache;
};

} // namespace registrar
} // namespace crate

#endif