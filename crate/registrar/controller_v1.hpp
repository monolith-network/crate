#ifndef CRATE_REGISTRAR_ENTRY_CONTROLLER_V1
#define CRATE_REGISTRAR_ENTRY_CONTROLLER_V1

#include <string>
#include <vector>
#include <tuple>
#include <crate/externals/simplejson/json.hpp>

/*
   About:
      A controller that can auto encode/decode to/from json/strings. This class
      is meant as a helper to standardize the submission and retrieval of controllers.
*/

namespace crate {
namespace registrar {

//! \brief A controller 
class controller_v1_c {
public:

   //! \brief An action representation
   struct action {
      std::string id;
      std::string description;
   };

   //! \brief Construct the controller
   //! \note Without setting the controller ID the data will be 
   //!       considered invalid. See `set_id`
   controller_v1_c();

   //! \brief Construct the controller with an id
   controller_v1_c(const std::string& id);

   //! \brief Decode controller_v1_c object from a json string
   //! \param json_data The string to build the controller from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode controller_v1_c object from a json object
   //! \param json_object The object to build the controller from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Set the controller id
   void set_id(const std::string& id);

   //! \brief Set the controller description
   void set_description(const std::string& description);

   //! \brief Add a new action
   //! \returns true iff the action has a unique id
   bool add_action(action new_action);

   //! \brief Retireve the id, description, and action list
   //!        of this controller
   std::tuple<std::string, std::string, std::vector<action> > get_data() const;

   //! \brief Clear all data from controller
   //! \post All data will be removed from controller
   void clear();

private:
   bool _invalid {true};
   void check_validity();
   std::string _id;
   std::string _description;
   std::string _ip;
   uint32_t _port {0};
   std::vector<action> _action_list;
};

} // namespace registrar
} // namespace crate

#endif