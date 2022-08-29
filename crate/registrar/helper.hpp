#ifndef CRATE_REGISTRAR_HELPER_HPP
#define CRATE_REGISTRAR_HELPER_HPP

#include <string>
#include <httplib.h>
#include <crate/registrar/node_v1.hpp>

namespace crate {
namespace registrar {

//! \brief A class that interfaces with the registrar and attempts
//!        to submit node data.
class helper_c {
public:

   //! \brief The results that can come from the node submitter
   enum class result {
      SUCCESS,
      REQUEST_EXCEEDS_URL_MAX_LENGTH,
      FAILURE,
      INVALID_NODE_DATA,
      UNABLE_TO_REACH_REGISTRAR,
      REGISTRAR_INTERNAL_ERROR,
      INTERNAL_ERROR,
      BAD_FETCH,
      NOT_FOUND
   };
   
   //! \brief Remove the default constructor
   helper_c() = delete;

   //! \brief Destroy helper_c
   ~helper_c();

   //! \brief Create the node submitter
   //! \param address The address of monolith
   //! \param port The port of monolith
   helper_c(const std::string& address, uint32_t port);

   //! \brief Attempt to submit v1 node data to the registrar
   //! \param node The node to push to the registrar
   //! \returns result of submission
   result submit(node_v1_c& node);

   //! \brief Attempt to retrieve a v1 node data from the registrar
   //! \param[in] id The id to retrieve 
   //! \param[out] node The node to retrieve from the registrar
   //! \returns result of retrieval, node should only be considered valid
   //!          if result = result::SUCCESS
   result retrieve(const std::string& id,  node_v1_c& node);

   //! \brief Attempt to remove a v1 node data from the registrar
   //! \param id The id to remove 
   //! \returns result of removal
   result remove(const std::string& id);

private:
   static constexpr uint16_t HTTP_URL_MAX_LEN = 2048;
   httplib::Client* _http_client {nullptr};
   result issue_command(const std::string& path);
};

} // namespace registrar
} // namespace crate

#endif
