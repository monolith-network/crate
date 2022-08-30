#ifndef CRATE_METRICS_HELPER_HPP
#define CRATE_METRICS_HELPER_HPP

#include <string>
#include <httplib.h>
#include <crate/metrics/heartbeat_v1.hpp>
#include <crate/metrics/reading_v1.hpp>
#include <crate/networking/message_writer.hpp>

namespace crate {
namespace metrics {

//! \brief A class that interfaces with the metrics endpoints
class helper_c {
public:

   //! \brief Type configuration to indicate which metric submission 
   //!        endpoint should be used
   enum class endpoint_type_e {
      TCP,
      HTTP
   };

   //! \brief The results that can come from the node submitter
   enum class result {
      SUCCESS,
      REQUEST_EXCEEDS_URL_MAX_LENGTH,
      FAILURE,
      INVALID_DATA,
      UNABLE_TO_REACH,
      MONOLITH_INTERNAL_ERROR,
      INTERNAL_ERROR,
      BAD_FETCH,
      WRITE_FAILED
   };
   
   //! \brief Remove the default constructor
   helper_c() = delete;

   //! \brief Destroy helper_c
   ~helper_c();

   //! \brief Create the node submitter
   //! \param address The address of monolith
   //! \param port The port of monolith
   helper_c(const endpoint_type_e type, const std::string& address, uint32_t port);

   //! \brief Attempt to submit v1 heartbeat data
   //! \param heartbeat The data to submit
   //! \returns result of submission
   result submit(heartbeat_v1_c& heartbeat);

   //! \brief Attempt to submit v1 reading data
   //! \param reading The data to submit
   //! \returns result of submission
   result submit(sensor_reading_v1_c& reading);

private:
   static constexpr uint16_t HTTP_URL_MAX_LEN = 2048;
   httplib::Client* _http_client {nullptr};
   crate::networking::message_writer_c* _tcp_writer {nullptr};
   result issue_command(const std::string& path);
   result write_tcp(const std::string& data);
};

} // namespace metrics
} // namespace crate

#endif
