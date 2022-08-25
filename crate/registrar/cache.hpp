#ifndef CRATE_REGISTRAR_CACHE_HPP
#define CRATE_REGISTRAR_CACHE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <set>
#include <chrono>
#include <tuple>

/*
   About:
      The cache hashes the node string and if it is given a sensor it combines the hash of 
      the sensor name. 

      Upon querying if an item exists the primary cache is checked to see if that hash exists,
      if it does not exist then it reaches out to the registrar to check for the information.

      If we are simply looking for a node, we use the /probe endpoint
      If we are looking for a node + sensor, we use the /fetch endpoint

      At any given time if a step fails, a cache::result is sent back to identify what
      happened. 

      If the item being searched for is found, then we cache it and store a cache_entry
      in _cache_items that will be used later during a `prune`

      The cache doesn't auto prune as the cache doesn't have enough information about
      the target application to know when the best time to prune is as pruning is expensive
      and locks the cache. For this reason, the caller must call prune to ensure that 
      false positives are not sent back.
*/

namespace crate {
namespace registrar {

//! \brief A class that interfaces with and caches results from the
//!        node registrar application for the sake of determining if
//!        a node exists, and if so, if a given sensor is registered as 
//!        being part of it and caches it for quicker access.
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
      FAILED_DECODE,
      BAD_FETCH
   };

   //! \brief Remove the default constructor
   cache() = delete;

   //! \brief Create the node cache
   //! \param registrar_address The address of the registrar database
   //! \param registrar_port The port of the registrar
   cache(const std::string& registrar_address, short registrar_port);

   //! \brief Check if a node exists
   //! \param node The node that we want to check if it exists
   //! \returns A result enumeration specifying if node is known or not
   result check_cache_for_node(const std::string& node);

   //! \brief Check if a node exists and if a specific sensor belongs to that node
   //! \param node The node that we want to check if it exists
   //! \param sensor The name of the sensor to check belongs to the node
   //! \returns A result enumeration specifying if everything is known, or if not, 
   //!          what isn't known
   result check_cache_for_node_sensor(const std::string& node, const std::string& sensor);

   //! \brief Set the time that the data will prune
   //! \param seconds The new prune time that will be used
   //! \post  The new prune time will be used to gauge when an 
   //!        item should be removed from the cache
   void set_prune_time(const double seconds);

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
         std::chrono::high_resolution_clock > retrieved; 
   };

   std::string _registrar_address;
   short _registrar_port;

   std::set<std::size_t> _primary_cache;
   std::vector<cache_entry> _cache_items;
   std::mutex _mutex;
   double _prune_time {DEFAULT_PRUNE_TIME_SEC};
   std::string _probe_nonce;

   result check_registrar(const size_t hash, 
                           const std::string& node, 
                           const std::string& sensor);
   result probe_registrar(const size_t hash, 
                           const std::string& node);
   void cache_item(const size_t hash);
};

} // namespace registrar
} // namespace crate

#endif