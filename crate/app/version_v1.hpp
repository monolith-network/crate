#ifndef CRATE_APP_VERSION_HPP
#define CRATE_APP_VERSION_HPP

#include "semver.hpp"

#include <tuple>
#include <string>
#include <crate/externals/simplejson/json.hpp>

namespace crate {
namespace app {


//! \brief A version representation
class version_v1_c {

public:
   //! \brief Create the version with no data set
   version_v1_c();

   //! \brief Create the version 
   //! \param name The application name
   //! \param hash The git has at the time of build
   //! \param semver The semantic version
   version_v1_c(std::string name,
                     std::string hash,
                     semver_s semver);

   //! \brief Decode version_v1_c object from a json string
   //! \param json_data The string to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(const std::string& json_data);

   //! \brief Decode version_v1_c object from a json object
   //! \param json_object The object to build the reading from
   //! \returns true iff the string could be fully decoded
   bool decode_from(json::jobject json_object);

   //! \brief Encode the object to a json string
   //! \param output_string The string to encode to 
   //! \returns true iff the item could be encoded to the a 
   //!          json string
   bool encode_to(std::string& output_string);

   //! \brief Retrieve the name, hash, and semantic cersion
   //! \returns A tuple of the values in the order listed above
   std::tuple<std::string, std::string, semver_s> get_data();

private:
   std::string _name;
   std::string _hash;
   semver_s _semver;
   bool _invalid {true};
   void check_valid();
};

} // namespace metrics
} // namespace crate

#endif