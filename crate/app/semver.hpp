#ifndef CRATE_APP_SEMVER_HPP
#define CRATE_APP_SEMVER_HPP

#include <string>

namespace crate {
namespace app {

   //! \brief The semanti
   struct semver_s {
      std::string major;
      std::string minor;
      std::string patch;
   };
} // namespace app
} // namespace crate

#endif