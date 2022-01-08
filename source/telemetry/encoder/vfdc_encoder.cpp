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
  for (const SensorVariantPair &pair : data) {
    size_t current_size;
    std::visit([&](auto v) { current_size = sizeof(v); }, std::get<1>(pair));
    size += current_size;
  }
  if (!(size % 4)) size += size % 4;
  size += size_t(data.size()) + 1;

  // First, let's push the number of sensor values we expect
  char compressed_data[size];
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
        [&](auto v)
        {
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

std::vector<SensorVariantPair> VFDCPEncoder::decode_data(CompressedData data) {
  // Let's decompress the data...
  std::vector<SensorVariantPair> values{};
  return values;
}

/* Unit Tests */
void test_one_value() {
  unsigned char id = 1;
  SensorDataVariant datum = int(1000);
  SensorVariantPair pair = { id, datum };
  std::vector<SensorVariantPair> data;
  data.push_back(pair);
  CompressedData compressed = VFDCPEncoder::get().encode_data(data);

  // Ensure our size is 6
  assert(std::get<1>(compressed) == 6);

  // Ensure out sensor count is 1
  char *encoding = std::get<0>(compressed);
  assert(encoding[0] == 1);

  // Ensure the first byte is the char id = 1
  assert(encoding[1] == 1);
  
  // Ensure the first value is 5
  int value = *(int*)(&encoding[2]);
  std::cout << value << std::endl;
  assert(value == 1000);
}

void test_multiple_values() {
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
}

int main() {
  test_one_value();
  test_multiple_values();
}