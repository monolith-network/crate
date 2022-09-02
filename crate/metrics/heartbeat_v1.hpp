#ifndef CRATE_METRICS_HEARTBEAT_HPP
#define CRATE_METRICS_HEARTBEAT_HPP

#include <string>
#include <crate/externals/simplejson/json.hpp>
#include <tuple>

namespace crate {
namespace metrics {

//! \brief A heartbeat reading off of a node
class heartbeat_v1_c {

public:
   //! \brief Create the heartbeat with no data set
   heartbeat_v1_c();

   //! \brief Create the heartbeat reading 
   //! \param id The id of the the node reporting data
   heartbeat_v1_c(std::string id);

   //! \brief Decode heartbeat_v1_c object from a json string
   //! \param json_data The string to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode heartbeat_v1_c object from a json object
   //! \param json_object The object to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Retrieve the id
   //! \returns A the id
   std::string get_data();

private:
   std::string _id;
   bool _invalid {true};
   void check_valid();
};

} // namespace metrics
} // namespace crate

#endif