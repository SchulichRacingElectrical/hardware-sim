/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "vfdc_encoder.h"

CompressedData VFDCPEncoder::encode_data(std::vector<SensorVariantPair>& data) {
  // Sort the variants from largest to smallest with respect to bytes.
  std::sort(data.begin(), data.end(),
    [](SensorVariantPair &a, SensorVariantPair &b) {
      size_t a_size = 0; 
      std::visit([&](auto v) { a_size = sizeof(v); }, std::get<1>(a));
      size_t b_size = 0;
      std::visit([&](auto v) { b_size = sizeof(v); }, std::get<1>(b));
      return a_size > b_size;
    });

  // Determine the amount of memory we need to allocate 
  // size = 1 + number of sensor ids + sum of data type sizes for current data + padding ([0, 3])
  size_t size = 0;
  for (const SensorVariantPair& pair : data) {
    size_t current_size;
    std::visit([&](auto v) { current_size = sizeof(v); size += current_size; }, std::get<1>(pair));
  }
  if (!(size % 4)) size += size % 4;
  size += size_t(data.size()) + 1;

  // First, let's push the number of sensor values we expect
  char* compressed_data = new char[size]; // NEED TO DELETE!
  char* temp = compressed_data;
  *temp = size_t(data.size());
  temp++;

  // Append the sensor ids after the size specifier
  for (const SensorVariantPair &pair : data) {
    *temp = std::get<0>(pair);
    temp++;
  }

  // Append the sensor values after the list of identifiers, in order
  for (const SensorVariantPair &pair : data) {
    std::visit(
      [&](auto v) {
        size_t size = sizeof(v);
        char* value = static_cast<char*>(static_cast<void*>(&v));
        for (size_t i = 0; i < size; i++) {
          *temp = *(value + i);
          temp++;
        }
      },
      std::get<1>(pair));
  }

  return CompressedData{ compressed_data, size };
}

std::vector<SensorVariantPair> VFDCPEncoder::decode_data(char* data, std::unordered_map<unsigned char, Sensor>& sensors) {
  // Get the sensor ids
  size_t sensor_count = data[0];
  unsigned char sensor_ids[sensor_count];
  for (int i = 1; i < sensor_count + 1; i++) {
    sensor_ids[i - 1] = data[i];
  }

  // Decode the data
  char *temp = data + sensor_count + 1;
  std::vector<SensorVariantPair> decoded{};
  for (const unsigned char& sensor_id: sensor_ids) {
    try {
      auto variant = sensors[sensor_id].get_variant();
      std::visit([&](auto v) { // Does this run async?
        size_t size = sizeof(v);
        char decoded_bytes[size];
        for (size_t i = 0; i < size; i++)
        {
          decoded_bytes[i] = *temp;
          temp++;
        }
        decltype(v) final_value = *static_cast<decltype(v)*>(static_cast<void*>(&decoded_bytes));
        decoded.emplace_back(sensor_id, final_value);
      }, variant);
    } catch (...) {
      throw std::runtime_error("Decoded sensor ID does not have a corresponding type.");
    }
  }

  return decoded;
}

#define TESTING 1
#if TESTING
#include <cassert>
#include <bitset>
#include <iostream>

static std::unordered_map<unsigned char, Sensor> test_sensors{};

void test_encode_decode() {
  std::vector<SensorVariantPair> data;
  long long first = 30;
  data.push_back({1, first}); // 8
  data.push_back({2, double(15)}); // 8
  data.push_back({3, float(10)}); // 4
  data.push_back({4, int(4)}); // 4
  data.push_back({5, short(3)}); // 2
  data.push_back({6, char(2)}); // 1
  data.push_back({7, bool(false)}); // 1
  CompressedData compressed = VFDCPEncoder::get().encode_data(data);
  
  // Ensure our size is 1 + 7 + 8 + 8 + 4 + 4 + 2 + 1 + 1 = 36 bytes
  assert(std::get<1>(compressed) == 36);

  // Let's decompress the data
  char *decoded = std::get<0>(compressed);
  std::vector<SensorVariantPair> uncompressed = VFDCPEncoder::get().decode_data(decoded, test_sensors);
  for (const auto& decode: uncompressed) {
    std::visit([](auto v)
               { std::cout << (double)v << std::endl; },
               std::get<1>(decode));
  }
  // TODO: Create sensors
  // Call decompression
  // Compare first and new vector
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
  char *encoding = std::get<0>(compressed);
  assert(encoding[0] == 1);

  // Ensure the first byte is the char id = 1
  assert(encoding[1] == 1);

  // Ensure the first value is 1000
  double value = *(double *)(&encoding[2]);
  assert(value == 1000);
}

int main() {
  test_sensors[1] = Sensor("A", 'q', 100, 5, 1, 1000, 0, -100, 100);
  test_sensors[2] = Sensor("B", 'd', 100, 5, 1, 1000, 0, -100, 100);
  test_sensors[3] = Sensor("C", 'f', 100, 5, 1, 1000, 0, -100, 100);
  test_sensors[4] = Sensor("D", 'i', 100, 5, 1, 1000, 0, -100, 100);
  test_sensors[5] = Sensor("E", 'h', 100, 5, 1, 1000, 0, -100, 100);
  test_sensors[6] = Sensor("F", 'c', 100, 5, 1, 1000, 0, -100, 100);
  test_sensors[7] = Sensor("G", '?', 100, 5, 1, 1000, 0, -100, 100);
  // test_one_value();
  test_encode_decode();
}
#endif