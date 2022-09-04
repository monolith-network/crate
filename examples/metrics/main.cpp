#include <iostream>
#include <crate/app/version_v1.hpp>
#include <crate/metrics/reading_v1.hpp>
#include <chrono>

int main(int argc, char** argv) {

   auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count();

   std::string node = "0000-0000-0000-0001";
   std::string sensor = "AAAA-AAAA-AAAA-AAAA";
   double value = 0.19374;

   auto reading = crate::metrics::sensor_reading_v1_c(
      ts,
      node,
      sensor,
      value
   );

   std::string encoded_string;
   if (!reading.encode_to(encoded_string)) {
      std::cout << "Failed to encode to string\n";
      std::exit(1);
   }

   std::cout << "Encoded string: " << encoded_string << std::endl;

   auto decoded = crate::metrics::sensor_reading_v1_c();
   if (!decoded.decode_from(encoded_string)) {
      std::cout << "Failed to decode from string\n";
      std::exit(1); 
   }

   auto [decoded_ts, decoded_node_id, decoded_sensor_id, decoded_value] = decoded.get_data();

   if (decoded_ts != ts) {
      std::cout << "Timestamp does not match\n";
      std::exit(1);
   }

   if (decoded_node_id != node) {
      std::cout << "Node id does not match\n";
      std::exit(1);
   }

   if (decoded_sensor_id != sensor) {
      std::cout << "Sensor id does not match\n";
      std::exit(1);
   }

   if (decoded_value != value) {
      std::cout << "Value does not match\n";
      std::exit(1);
   }

   crate::app::version_v1_c version("Monolith", "Some-hash", {"0", "0", "0"});

   std::string v_encoded;
   if (!version.encode_to(v_encoded)) {
      std::cout << "Failed to encode version" << std::endl;
      std::exit(1);
   }

   crate::app::version_v1_c v_decoded;
   if (!v_decoded.decode_from(v_encoded)) {
      std::cout << "Failed to decode version" << std::endl;
      std::exit(1);
   }

   auto [n, h, s] = version.get_data();
   auto [a, b, c] = v_decoded.get_data();

   std::cout << "Encoded version: " << v_encoded << std::endl;

   if (n != a) {
      std::cout << "Version name doesn't match" << std::endl;
      std::exit(1);
   } 
   if (h != b) {
      std::cout << "Version hash doesn't match" << std::endl;
      std::exit(1);
   } 
   if (s.major != c.major || s.minor != c.minor || s.patch != c.patch) {
      std::cout << "Version semver doesn't match" << std::endl;
      std::exit(1);
   }

   std::cout << "Everything worked" << std::endl;
   return 0;
}