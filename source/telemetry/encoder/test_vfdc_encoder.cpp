/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#define TESTING 1
#if TESTING
#include "vfdc_encoder.h"
#include "../thing/sensor.h"
#include <cassert>
#include <bitset>
#include <iostream>
#include <unordered_map>

static std::unordered_map<unsigned char, Sensor> test_sensors{};

void test_encode_decode() {
  std::vector<SensorVariantPair> data;
  long long first = 30;
  data.push_back({1, first}); // 8
  data.push_back({2, double(15)}); // 8
  data.push_back({3, float(10)}); // 4
  data.push_back({4, int(4)}); // 4
  data.push_back({5, short(3)}); // 2
  data.push_back({6, char(97)}); // 1
  data.push_back({7, bool(false)}); // 1
  CompressedData compressed = VFDCPEncoder::get().encode_data(data);
  
  // Ensure our size is 1 + 7 + 8 + 8 + 4 + 4 + 2 + 1 + 1 = 36 bytes
  assert(std::get<1>(compressed) == 36);

  // Let's decompress the data
  auto decoded = std::get<0>(compressed);
  std::vector<SensorVariantPair> uncompressed = VFDCPEncoder::get().decode_data(decoded, test_sensors);
  for (const auto& decode: uncompressed) {
    std::visit([](auto v) { std::cout << v << std::endl; }, std::get<1>(decode));
  }
}

void test_one_value()
{
  unsigned char id = 1;
  SensorDataVariant datum = double(1000);
  SensorVariantPair pair = {id, datum};
  std::vector<SensorVariantPair> data;
  data.push_back(pair);
  CompressedData compressed = VFDCPEncoder::get().encode_data(data);

  // Ensure our size is 6
  assert(std::get<1>(compressed) == 10);

  // Ensure out sensor count is 1
  auto encoding = std::get<0>(compressed);
  assert(encoding[0] == 1);

  // Ensure the first byte is the char id = 1
  assert(encoding[1] == 1);

  // Ensure the first value is 1000
  double value = *(double *)(&encoding[2]);
  assert(value == 1000);
}

// int main() {
//   test_sensors[1] = Sensor("A", 'q', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_sensors[2] = Sensor("B", 'd', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_sensors[3] = Sensor("C", 'f', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_sensors[4] = Sensor("D", 'i', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_sensors[5] = Sensor("E", 'h', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_sensors[6] = Sensor("F", 'c', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_sensors[7] = Sensor("G", '?', 100, 5, 1, {-100, 100}, {-100, 100});
//   test_one_value();
//   test_encode_decode();
// }
#endif