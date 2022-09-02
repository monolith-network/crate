#ifndef CRATE_METRICS_READING_HPP
#define CRATE_METRICS_READING_HPP

#include <string>
#include <crate/externals/simplejson/json.hpp>
#include <tuple>

namespace crate {
namespace metrics {

//! \brief A sensor reading off of a node
class sensor_reading_v1_c {

public:
   //! \brief Create the sensor with no data set
   sensor_reading_v1_c();

   //! \brief Create the sensor reading 
   //! \param timestamp Seconds since epoch
   //! \param id The id of the the node reporting data
   //! \param sensor_id The id of the sensor
   //! \param value The _value from the sensor
   sensor_reading_v1_c(int64_t timestamp, 
                     std::string id, 
                     std::string sensor_id, 
                     double value);

   //! \brief Set the time stamp to the time of this call
   //! \post the timestamp will be set to the ms since epoch
   void stamp();

   //! \brief Decode sensor_reading_v1_c object from a json string
   //! \param json_data The string to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode sensor_reading_v1_c object from a json object
   //! \param json_object The object to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Retrieve the timestamp, node_id, sensor_id, and _value
   //! \returns A tuple of the values in the order listed above
   std::tuple<int64_t, std::string, std::string, double> get_data();

private:
   int64_t _timestamp {0};
   std::string _node_id;
   std::string _sensor_id;
   double _value {0.0};
   bool _invalid {true};
   void check_valid();
};

} // namespace metrics
} // namespace crate

#endif