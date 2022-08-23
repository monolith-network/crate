#include <iostream>
#include <crate/externals/aixlog/logger.hpp>
#include <crate/common/common.hpp>
#include <crate/registrar/cache.hpp>

int main(int argc, char** argv) {

   crate::common::setup_logger("cacheApp", AixLog::Severity::debug);
   crate::registrar::cache registrar_cache("0.0.0.0", 4096);

   switch(registrar_cache.check_cache_for_node_sensor("node_0", "sensor_0")) {
      case crate::registrar::cache::result::KNOWN: 
         std::cout << "KNOWN\n"; 
      break;
      case crate::registrar::cache::result::UNKNOWN_NODE: 
         std::cout << "UNKNOWN_NODE\n"; 
      break;
      case crate::registrar::cache::result::UNKNOWN_NODE_SENSOR: 
         std::cout << "UNKNOWN_NODE_SENSOR\n"; 
      break;
      case crate::registrar::cache::result::UNABLE_TO_REACH_REGISTRAR: 
         std::cout << "UNABLE_TO_REACH_REGISTRAR\n"; 
      break;
   }



   return 0;
}