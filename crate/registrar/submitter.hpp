#ifndef CRATE_REGISTRAR_SUBMITTER_HPP
#define CRATE_REGISTRAR_SUBMITTER_HPP

#include <crate/registrar/entry/node_v1.hpp>

/*
   About:
      Submits a given node entity to the registrar
*/

namespace crate {
namespace registrar {

//! \brief A class that interfaces with the registrar and attempts
//!        to submit node data.
class submitter {
public:

   //! \brief The results that can come from the node submitter
   enum class result {
      SUCCESS,
      SUBMISSION_FAILURE,
      INVALID_NODE_DATA,
      UNABLE_TO_REACH_REGISTRAR,
      REGISTRAR_INTERNAL_ERROR,
      SUBMIT_INTERNAL_ERROR,
      BAD_FETCH
   };
   
   //! \brief Remove the default constructor
   submitter() = delete;

   //! \brief Create the node submitter
   //! \param registrar_address The address of the registrar database
   //! \param registrar_port The port of the registrar
   submitter(const std::string& registrar_address, short registrar_port);

   //! \brief Attempt to submit v1 node data to the registrar
   //! \param node The node to push to the registrar
   //! \returns result of submission
   result submit(node_v1& node);

private:
   std::string _registrar_address;
   short _registrar_port;
};

} // namespace registrar
} // namespace crate

#endif