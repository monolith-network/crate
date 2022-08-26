#ifndef CRATE_METRICS_STREAM_RECEIVER_HPP
#define CRATE_METRICS_STREAM_RECEIVER_HPP

#include <tuple>
#include <vector>
#include <crate/metrics/streams/stream_data.hpp>

namespace crate {
namespace metrics {
namespace streams {

//! \brief Receiver interface for something that takes in and processes stream data
class stream_receiver_if {
public:
   //! \brief Receive a chunk of stream data
   //! \param data The data that was received and needs to be processed
   virtual void receive_data(stream_data_v1 data) = 0;
};

} // namespace stream
} // namespace metrics
} // namespace crate

#endif