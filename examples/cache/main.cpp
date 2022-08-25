#include <iostream>
#include <crate/externals/aixlog/logger.hpp>
#include <crate/externals/uuid4/uuid4.h>
#include <crate/common/common.hpp>
#include <crate/registrar/cache.hpp>
#include <crate/registrar/submitter.hpp>
#include <crate/registrar/retriever.hpp>
#include <crate/registrar/entry/node_v1.hpp>
#include <thread>

/*
   ABOUT:
      This is a test/example. 
      Creates a node, submits it to the registrar. 
      Retrieves that node using the cache
      Prunes the cache
      re-retrieves that node
*/

using namespace std::chrono_literals;

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
      case crate::registrar::cache::result::REGISTRAR_INTERNAL_ERROR: 
         std::cout << "REGISTRAR_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::cache::result::CACHE_INTERNAL_ERROR: 
         std::cout << "CACHE_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::cache::result::FAILED_DECODE: 
         std::cout << "FAILED_DECODE\n"; 
      break;
      case crate::registrar::cache::result::BAD_FETCH: 
         std::cout << "BAD_FETCH\n"; 
      break;
   }
}

void probe_item(crate::registrar::cache& registrar_cache, std::string node)
{
   switch(registrar_cache.check_cache_for_node(node)) {
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
      case crate::registrar::cache::result::REGISTRAR_INTERNAL_ERROR: 
         std::cout << "REGISTRAR_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::cache::result::CACHE_INTERNAL_ERROR: 
         std::cout << "CACHE_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::cache::result::BAD_FETCH: 
         std::cout << "BAD_FETCH\n"; 
      break;
   }
}

void retrieve_item(crate::registrar::retriever& retriever, std::string id, crate::registrar::node_v1& node)
{
   switch(retriever.retrieve(id, node)) {
      case crate::registrar::retriever::result::SUCCESS: 
         std::cout << "SUCCESS\n"; 
      break;
      case crate::registrar::retriever::result::NOT_FOUND: 
         std::cout << "NOT_FOUND\n"; 
      break;
      case crate::registrar::retriever::result::INVALID_NODE_DATA: 
         std::cout << "INVALID_NODE_DATA\n"; 
      break;
      case crate::registrar::retriever::result::UNABLE_TO_REACH_REGISTRAR: 
         std::cout << "UNABLE_TO_REACH_REGISTRAR\n"; 
      break;
      case crate::registrar::retriever::result::REGISTRAR_INTERNAL_ERROR: 
         std::cout << "REGISTRAR_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::retriever::result::RETRIEVER_INTERNAL_ERROR: 
         std::cout << "RETRIEVER_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::retriever::result::BAD_FETCH: 
         std::cout << "BAD_FETCH\n"; 
      break;
   }
}

void submit_node(crate::registrar::submitter& submitter, crate::registrar::node_v1& node) {

   switch(submitter.submit(node)) {
      case crate::registrar::submitter::result::SUCCESS: 
         std::cout << "SUCCESS\n"; 
      break;
      case crate::registrar::submitter::result::SUBMISSION_FAILURE: 
         std::cout << "SUBMISSION_FAILURE\n"; 
      break;
      case crate::registrar::submitter::result::INVALID_NODE_DATA: 
         std::cout << "INVALID_NODE_DATA\n"; 
      break;
      case crate::registrar::submitter::result::UNABLE_TO_REACH_REGISTRAR: 
         std::cout << "UNABLE_TO_REACH_REGISTRAR\n"; 
      break;
      case crate::registrar::submitter::result::REGISTRAR_INTERNAL_ERROR: 
         std::cout << "REGISTRAR_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::submitter::result::SUBMIT_INTERNAL_ERROR: 
         std::cout << "SUBMIT_INTERNAL_ERROR\n"; 
      break;
      case crate::registrar::submitter::result::BAD_FETCH: 
         std::cout << "BAD_FETCH\n"; 
      break;
   }
}

int main(int argc, char** argv) {

   char buf[UUID4_LEN];
   uuid4_init();
   uuid4_generate(buf);

   std::string id(buf);
   std::cout << "Node id: " << id << std::endl;

   crate::registrar::node_v1 node;
   node.set_id(id);
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

   std::cout << "node: " << encoded_node << std::endl;

   crate::registrar::node_v1 decoded_node;
   if (!decoded_node.decode_from(encoded_node)) {
      std::cerr << "Failed to decode the node\n";
      std::exit(1);
   }

   // Ensure encode/decode worked
   {
      auto [node_one_id, node_one_desc, node_one_sensors] = node.get_data();
      auto [node_two_id, node_two_desc, node_two_sensors] = decoded_node.get_data();

      if (node_one_id != node_two_id) {
         std::cerr << "Ids failed match {" << node_one_id << ", " << node_two_id << " }" << std::endl;
      }
      if (node_one_desc != node_two_desc) {
         std::cerr << "Descs failed match {" << node_one_desc << ", " << node_two_desc << " }" << std::endl;
      }
   }

   crate::common::setup_logger("cacheApp", AixLog::Severity::debug);

   crate::registrar::submitter registrar_submitter("0.0.0.0", 4096);

   
   std::cout << "[SUBMIT NODE]" << std::endl;
   submit_node(registrar_submitter, node);


   std::cout << "[CHECK AND CACHE - 0]" << std::endl;

   crate::registrar::cache registrar_cache("0.0.0.0", 4096);

   // Set prune time to 1 second to ensure we prune
   registrar_cache.set_prune_time(1.0);

   get_item(registrar_cache, id, "0000-0000-0001");  // Should exist
   get_item(registrar_cache, id, "0000-0000-0000");  // Should exist
   get_item(registrar_cache, id, "0000-0000-0001");  // Should be cached
   get_item(registrar_cache, id, "0000-0000-0002");  // No exist

   // Sleep for 2s
   std::this_thread::sleep_for(2s);

   std::cout << "[PRUNE - 0]" << std::endl;

   // Prune
   registrar_cache.prune();

   std::cout << "[CHECK AND CACHE - 1]" << std::endl;

   // Re-retrieve thing
   get_item(registrar_cache, id, "0000-0000-0001");  // Shouldn't exist anymore
   get_item(registrar_cache, id, "0000-0000-0001"); 
   get_item(registrar_cache, id, "0000-0000-0001"); 

   std::this_thread::sleep_for(2s);

   std::cout << "[PRUNE - 1]" << std::endl;
   
   registrar_cache.prune();

   std::cout << "[PROBE]" << std::endl;

   probe_item(registrar_cache, id); 
   probe_item(registrar_cache, id); 
   probe_item(registrar_cache, id + id); 

   std::cout << "[RETRIEVE AND COMPARE NODE]" << std::endl;

   crate::registrar::node_v1 retrieved_node;
   crate::registrar::retriever registrar_retriever("0.0.0.0", 4096);

   retrieve_item(registrar_retriever, id, retrieved_node);

   auto [node_one_id, node_one_desc, node_one_sensors] = node.get_data();
   auto [retrieved_id, retrieved_desc, retrieved_sensors] = decoded_node.get_data();

   if (node_one_id != retrieved_id) {
      std::cerr << "Ids failed match {" << node_one_id << ", " << retrieved_id << " }" << std::endl;
   }
   if (node_one_desc != retrieved_desc) {
      std::cerr << "Descs failed match {" << node_one_desc << ", " << retrieved_desc << " }" << std::endl;
   }

   return 0;
}