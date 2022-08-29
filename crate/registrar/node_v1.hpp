#ifndef CRATE_REGISTRAR_ENTRY_NODE_V1
#define CRATE_REGISTRAR_ENTRY_NODE_V1

#include <string>
#include <vector>
#include <tuple>
#include <crate/externals/simplejson/json.hpp>

/*
   About:
      A node that can auto encode/decode to/from json/strings. This class
      is meant as a helper to standardize the submission and retrieval of nodes.
*/

namespace crate {
namespace registrar {

//! \brief A node 
class node_v1_c {
public:

   //! \brief A sensor representation
   struct sensor {
      std::string id;
      std::string type;
      std::string description;
   };

   //! \brief Construct the node
   //! \note Without setting the node ID the data will be 
   //!       considered invalid. See `set_id`
   node_v1_c();

   //! \brief Construct the node with an id
   node_v1_c(const std::string& id);

   //! \brief Decode node_v1_c object from a json string
   //! \param json_data The string to build the node from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode node_v1_c object from a json object
   //! \param json_object The object to build the node from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Set the node id
   void set_id(const std::string& id);

   //! \brief Set the node description
   void set_description(const std::string& description);

   //! \brief Add a new sensor
   //! \returns true iff the sensor has a unique id
   bool add_sensor(sensor new_sensor);

   //! \brief Retireve the id, description, and sensor list
   //!        of this node
   std::tuple<std::string, std::string, std::vector<sensor> > get_data() const;

   //! \brief Clear all data from node
   //! \post All data will be removed from node
   void clear();

private:
   bool _invalid {true};
   void check_validity();
   std::string _id;
   std::string _description;
   std::vector<sensor> _sensor_list;
};

} // namespace registrar
} // namespace crate

#endif