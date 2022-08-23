#include "common.hpp"

namespace crate {
namespace common {

void setup_logger(const std::string& name, AixLog::Severity level) {
  auto sink_cout =
      std::make_shared<AixLog::SinkCout>(level, "[#severity] (#tag) #message");
  auto sink_file = std::make_shared<AixLog::SinkFile>(
      level, name + ".log",
      "%Y-%m-%d %H-%M-%S.#ms | [#severity] (#tag) #message");
  AixLog::Log::init({sink_cout, sink_file});
}

} // namespace common
} // namespace crate