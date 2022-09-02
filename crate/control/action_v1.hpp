#ifndef CRATE_CONTROL_ACTION_HPP
#define CRATE_CONTROL_ACTION_HPP

#include <string>
#include <crate/externals/simplejson/json.hpp>
#include <tuple>

namespace crate {
namespace metrics {

//! \brief A action reading off of a node
class controller_action_v1_c {

public:
   //! \brief Create the action with no data set
   controller_action_v1_c();

   //! \brief Create the action reading 
   //! \param timestamp Seconds since epoch
   //! \param id The id of the the node reporting data
   //! \param action_id The id of the action
   //! \param value The value for the action
   controller_action_v1_c(int64_t timestamp, 
                     std::string id, 
                     std::string action_id, 
                     double value);

   //! \brief Set the time stamp to the time of this call
   //! \post the timestamp will be set to the ms since epoch
   void stamp();

   //! \brief Decode controller_action_v1_c object from a json string
   //! \param json_data The string to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode controller_action_v1_c object from a json object
   //! \param json_object The object to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Retrieve the timestamp, controller_id, action_id, and _value
   //! \returns A tuple of the values in the order listed above
   std::tuple<int64_t, std::string, std::string, double> get_data();

private:
   int64_t _timestamp {0};
   std::string _controller_id;
   std::string _action_id;
   double _value {0.0};
   bool _invalid {true};
   void check_valid();
};

} // namespace metrics
} // namespace crate

#endif