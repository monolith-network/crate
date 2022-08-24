#include <iostream>
#include <crate/externals/aixlog/logger.hpp>
#include <crate/common/common.hpp>
#include <crate/registrar/cache.hpp>
#include <crate/registrar/entry/node_v1.hpp>

void get_item(crate::registrar::cache& registrar_cache, std::string node, std::string sensor)
{
   switch(registrar_cache.check_cache_for_node_sensor(node, sensor)) {
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
}

int main(int argc, char** argv) {

   crate::registrar::node_v1 node;
   node.set_id("house_node");
   node.set_description("A node representing the whole house");
   if (!node.add_sensor({
      "0000-0000-0000",
      "temperature",
      "Pantry temperature sensor"
   })) {
      std::cerr << "Failed to add sensor " << std::endl;
   }
   if (!node.add_sensor({
      "0000-0000-0001",
      "humidity",
      "Basement humidity sensor"
   })) {
      std::cerr << "Failed to add sensor " << std::endl;
   }

   std::string encoded_node;
   if (!node.encode_to(encoded_node)) {
      std::cerr << "Failed to encode node\n";
      std::exit(1);
   }

   crate::registrar::node_v1 decoded_node;
   if (!decoded_node.decode_from(encoded_node)) {
      std::cerr << "Failed to decode the node\n";
      std::exit(1);
   }

   auto [node_one_id, node_one_desc, node_one_sensors] = node.get_data();
   auto [node_two_id, node_two_desc, node_two_sensors] = decoded_node.get_data();

   if (node_one_id != node_two_id) {
      std::cerr << "Ids failed match {" << node_one_id << ", " << node_two_id << " }" << std::endl;
   }
   if (node_one_desc != node_two_desc) {
      std::cerr << "Descs failed match {" << node_one_desc << ", " << node_two_desc << " }" << std::endl;
   }

   crate::common::setup_logger("cacheApp", AixLog::Severity::debug);
   crate::registrar::cache registrar_cache("0.0.0.0", 4096);

   get_item(registrar_cache, "house_node", "0000-0000-0001");  // Should exist
   get_item(registrar_cache, "house_node", "0000-0000-0000");  // Should exist
   get_item(registrar_cache, "house_node", "0000-0000-0001");  // Should be cached
   get_item(registrar_cache, "house_node", "0000-0000-0002");  // No exist

   return 0;
}