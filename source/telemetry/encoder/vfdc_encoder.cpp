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
  // size = number of sensor ids + sum of data type sizes for current data + padding ([0, 3])
  size_t size = 0;
  size += size_t(data.size());
  for (const SensorVariantPair &pair : data) {
    size_t current_size;
    std::visit([&](auto v) { current_size = sizeof(v); }, std::get<1>(pair));
    size += current_size;
  }
  if (!(size % 4)) size += size % 4;
  
  // Append the sensor ids at the beginning of the byte array
  char compressed_data[size];
  char* temp = compressed_data;
  for (const SensorVariantPair &pair : data) {
    *temp = std::get<0>(pair);
    temp++;
  }

  // Append all of the sensor data
  for (const SensorVariantPair &pair : data) {
    std::visit(
      [&](auto v) { 
        *temp = v;
        temp += sizeof(v);
      }, 
      std::get<1>(pair)
    );
  }

  return CompressedData{ compressed_data, size };
}

/* Unit Tests */
void test_one_value() {
  unsigned char id = 1;
  SensorDataVariant datum = int(5);
  SensorVariantPair pair = { id, datum };
  std::vector<SensorVariantPair> data;
  data.push_back(pair);
  CompressedData compressed = VFDCPEncoder::get().encode_data(data);

  // Ensure our size is 5
  assert(std::get<1>(compressed) == 5);

  // Ensure the first byte is the char id = 1
  char *encoding = std::get<0>(compressed);
  assert(encoding[0] == 1);
  
  // Ensure the first value is 5
  int value = encoding[1];
  assert(value == 5);
}

// void test_multiple_values() {
//   std::vector<SensorVariantPair> data;
//   data.push_back({ 1, long long()})
// }

int main() {
  test_one_value();
}