#ifndef CRATE_METRICS_STREAM_HELPER_HPP
#define CRATE_METRICS_STREAM_HELPER_HPP

#include <string>
#include <httplib.h>

namespace crate {
namespace metrics {
namespace streams {

class helper {
public:
   //! \brief The results that can come from the node submitter
   enum class result {
      SUCCESS,
      UNABLE_TO_REACH_MONOLITH,
      FAILED_TO_PARSE_RESPONSE,
      INTERNAL_SERVER_ERROR,
      UNKNOWN_ERROR,
      BAD_REQUEST
   };

   //! \brief Remove the default constructor
   helper() = delete;

   //! \brief Destroy the helper
   ~helper();

   //! \brief Create the node submitter
   //! \param address The address of monolith
   //! \param port The port of the monolith
   helper(const std::string& address, uint32_t port);

   //! \brief Register an address and port as a metric stream receiver
   //! \param address The address to register as a receiver
   //! \param port The port of to register on the receiver
   //! \returns result based ability to reach, and register with monolith
   result register_as_metric_stream_receiver(const std::string& address, uint32_t port);

   //! \brief Deregister an address and port as a metric stream receiver
   //! \param address The address to deregister as a receiver
   //! \param port The port of to deregister on the receiver
   //! \returns result based ability to reach, and register with monolith
   result deregister_as_metric_stream_receiver(const std::string& address, uint32_t port);

private:
   httplib::Client* _http_client {nullptr};
   result issue_command(const std::string& path);
};

} // namespace stream
} // namespace metrics
} // namespace crate
#endif