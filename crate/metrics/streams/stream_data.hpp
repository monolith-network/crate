#ifndef CRATE_METRICS_STREAM_DATA_HPP
#define CRATE_METRICS_STREAM_DATA_HPP

#include <tuple>
#include <vector>
#include <crate/metrics/reading.hpp>

namespace crate {
namespace metrics {
namespace streams {

/*
{
  "timestamp": 1661534969,
  "sequence": 0,
  "data": [
       {
          "timestamp": 1661463750264,
          "node_id": "asoidj-askdaskl-aslkdmasd-asda",
          "sensor_id": "aksldma-asdasdas-asdasd-asdasd",
          "value": 0.0123
       },
       {
          "timestamp": 1661463750274,
          "node_id": "asoidj-askdaskl-aslkdmasd-asda",
          "sensor_id": "aksldma-asdasdas-asdasd-asdasd",
          "value": 0.0123
       }
  ]
}
*/

//! \brief  A chunk of stream data made up of a group of metrics
//!         meant to be sent in a sequence to a destination.
//! \note   For any chunk of stream data to be valid there MUST be
//!         at least one metric in it, and the timestamp must be set.
//!         No stream server shall send invalid stream data, though
//!         that does not mean that invalid data can not be received.
//!          - The network is a dangerous place for data
class stream_data_v1 {
public:
   //! \brief Create the stream data with no data set
   stream_data_v1() {};

   //! \brief Construct a stream with no data
   //! \param sequence The sequence number of the stream data
   stream_data_v1(uint64_t sequence);

   //! \brief Construct a stream with data
   //! \param metrics The data to create the stream with
   //! \param sequence The sequence number of the stream data
   stream_data_v1(uint64_t sequence, 
                  std::vector<crate::metrics::sensor_reading_v1> metrics);

   //! \brief Set the time stamp to the time of this call
   //! \post the timestamp will be set to the ms since epoch
   void stamp();

   //! \brief Add a single metric to the stream
   //! \param metric The metric to add to the stream
   void add_metric(crate::metrics::sensor_reading_v1 metric);
   
   //! \brief Decode stream_data_v1 object from a json string
   //! \param json_data The string to build the data from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode stream_data_v1 object from a json object
   //! \param json_object The object to build the data from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Retrieve the size of the stream data
   //! \returns The count of metrics within the dataset
   size_t size() const;

   //! \brief Check if the data set contians metrics
   //! \returns true iff there are no metrics
   bool empty() const;

   //! \brief Retrieve the timestamp, sequence, and metric data
   //! \returns timestamp, sequence, and metric data in that order
   std::tuple<int64_t, 
               uint64_t, 
               std::vector<crate::metrics::sensor_reading_v1>> get_data();
private:
   int64_t _timestamp {0};
   uint64_t _sequence {0};
   std::vector<crate::metrics::sensor_reading_v1> _data;

   // Tiemstamp must be set, and _data must have metrics
   bool _invalid {true};
   void check_validity();
};

} // namespace stream
} // namespace metrics
} // namespace crate

#endif