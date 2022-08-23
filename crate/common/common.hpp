#ifndef CRATE_COMMON_HPP
#define CRATE_COMMON_HPP

#include <string>
#include <crate/externals/aixlog/logger.hpp>

namespace crate {
namespace common {

//! \brief Setup the external aixlogger in a standard way
//! \param name The name of the application setting up the logger
//! \param level The level to set the logger to
//! \post The logger will be setup to log at the given level, and the logs
//!       will also be written to a file named `name`.log
extern void setup_logger(const std::string& name, AixLog::Severity level);

} // namespace common
} // namespace crate



#endif