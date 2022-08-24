#ifndef CRATE_REGISTRAR_CACHE_HPP
#define CRATE_REGISTRAR_CACHE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <set>
#include <chrono>
#include <tuple>

namespace crate {
namespace registrar {

//! \brief A class that interfaces with and caches results from the
//!        node registrar application for the sake of determining if
//!        a node exists, and if so, if a given sensor is registered as 
//!        being part of it.
class cache {
public:

   static constexpr double DEFAULT_PRUNE_TIME_SEC = 60.0; //! Default prune time

   //! \brief The results that can come from the node cache
   enum class result {
      KNOWN,
      UNKNOWN_NODE,
      UNKNOWN_NODE_SENSOR,
      UNABLE_TO_REACH_REGISTRAR,
      REGISTRAR_INTERNAL_ERROR,
      CACHE_INTERNAL_ERROR,
      BAD_FETCH
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

   //! \brief Update the time that the data will prune
   //! \param seconds The new prune time that will be used
   //! \post  The new prune time will be used to gauge when an 
   //!        item should be removed from the cache
   void update_prune_time(const double seconds);

   //! \brief Run through the known items and perform a prune of old data
   //!        WARNING: This is an expensive operation, but it is necessary. 
   //!                 It is suggested that this method is called at the same
   //!                 rate as the configured prune time
   //! \post  Out of date information will be removed to ensure that the cache stays
   //!        up to date
   void prune();

   //! \brief Retrieve the configured prune time
   //! \returns The prune time (in seconds) that is being used to remove cache entries
   double get_prune_time() const;

private:
   struct cache_entry {
      std::size_t hash;
      std::chrono::time_point<
         std::chrono::system_clock> retrieved; 
   };

   std::string _registrar_address;
   short _registrar_port;

   std::set<std::size_t> _primary_cache;
   std::vector<cache_entry> _cache_items;
   std::mutex _mutex;
   double _prune_time {DEFAULT_PRUNE_TIME_SEC};

   result check_registrar(const size_t hash, 
                           const std::string& node, 
                           const std::string& sensor);
};

} // namespace registrar
} // namespace crate

#endif