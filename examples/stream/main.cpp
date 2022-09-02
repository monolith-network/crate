#include <iostream>
#include <vector>
#include <crate/metrics/reading.hpp>
#include <crate/metrics/streams/stream_data.hpp>
#include <crate/externals/uuid4/uuid4.h>
#include <crate/common/common.hpp>

namespace {

   std::vector<crate::metrics::sensor_reading_v1> readings;
}

void populate_readings(size_t num) {

   for (size_t i = 0; i < num; i++) {

      // Generate sensor id
      uuid4_init();
      char buffer[UUID4_LEN];
      uuid4_generate(buffer);

      crate::metrics::sensor_reading_v1 reading(0, "node_1", std::string(buffer), (double)i + 1.5);
      reading.stamp();

      readings.push_back(reading);
   }
}

int main(int argc, char** argv) {

   crate::common::setup_logger("stream", AixLog::Severity::debug);

   populate_readings(4);

   crate::metrics::streams::stream_data_v1 stream(0 ,readings);
   stream.stamp();

   std::string encoded;
   if (!stream.encode_to(encoded)) {
      std::cout << "Failed to encode stream" << std::endl;
      std::exit(1);
   }

   std::cout << encoded << std::endl;

   crate::metrics::streams::stream_data_v1 decoded_stream;

   if (!decoded_stream.decode_from(encoded)) {
      std::cout << "Failed to decode from string" << std::endl;
   }

   auto [timestamp, seq, data] = stream.get_data();
   auto [_timestamp, _seq, _data] = decoded_stream.get_data();

   if (timestamp != _timestamp) {
      std::cerr << "Timestamps don't match" << std::endl;
      std::exit(1);
   }

   if (seq != _seq) {
      std::cerr << "Sequences don't match" << std::endl;
      std::exit(1);
   }

   if (data.size() != _data.size()) {
      std::cerr << "Data not same size" << std::endl;
      std::exit(1);
   }

   for (auto i = 0; i < data.size(); i++) {
      auto [ts, n, s, v] = data[i].get_data();
      auto [_ts, _n, _s, _v] = _data[i].get_data();

      if (ts != _ts) {
         std::cerr << "Data item " << i << " did not have matching timestamps" << std::endl;
         std::exit(1);
      }
      
      if (n != _n) {
         std::cerr << "Data item " << i << " did not have matching node ids" << std::endl;
         std::exit(1);
      }
      
      if (s != _s) {
         std::cerr << "Data item " << i << " did not have matching sensor ids" << std::endl;
         std::exit(1);
      }
      
      if (v != _v) {
         std::cerr << "Data item " << i << " did not have matching values" << std::endl;
         std::exit(1);
      }
   }

   
   std::string de_encoded;
   if (!decoded_stream.encode_to(de_encoded)) {
      std::cout << "Failed to encode decoded stream" << std::endl;
      std::exit(1);
   }

   std::cout << "-------------------------------------------\n";
   std::cout << de_encoded << std::endl;
   std::cout << "-------------------------------------------\n";

   std::cout << "SUCCESS" << std::endl;
   return 0;
}