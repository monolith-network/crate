#ifndef CRATE_REGISTRAR_RETRIEVER_HPP
#define CRATE_REGISTRAR_RETRIEVER_HPP

#include <string>
#include <crate/registrar/entry/node_v1.hpp>

/*
   About:
      Retrieves a node from the registrar and populates it as the 
      given node entity
*/

namespace crate {
namespace registrar {

//! \brief A class that interfaces with the registrar and attempts
//!        to retrieve node data.
class retriever {
public:

   //! \brief The results that can come from the node retriever
   enum class result {
      SUCCESS,
      NOT_FOUND,
      INVALID_NODE_DATA,
      FAILED_DECODE,
      UNABLE_TO_REACH_REGISTRAR,
      REGISTRAR_INTERNAL_ERROR,
      RETRIEVER_INTERNAL_ERROR,
      BAD_FETCH
   };
   
   //! \brief Remove the default constructor
   retriever() = delete;

   //! \brief Create the node retriever
   //! \param registrar_address The address of the registrar database
   //! \param registrar_port The port of the registrar
   retriever(const std::string& registrar_address, short registrar_port);

   //! \brief Attempt to retrieve a v1 node data from the registrar
   //! \param[in] id The id to retrieve 
   //! \param[out] node The node to retrieve from the registrar
   //! \returns result of retrieval, node should only be considered valid
   //!          if result = result::SUCCESS
   result retrieve(const std::string& id,  node_v1& node);

private:
   std::string _registrar_address;
   short _registrar_port;
};

} // namespace registrar
} // namespace crate

#endif