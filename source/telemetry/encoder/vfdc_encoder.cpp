/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "vfdc_encoder.h"

std::vector<char> VFDCPEncoder::encode_data(std::vector<SensorVariantPair>& data) {
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
  int index = 0;
  std::vector<char> compressed_data(size, 0);
  compressed_data[index] = size_t(data.size());
  index++;

  // Append the sensor ids after the size specifier
  for (const SensorVariantPair &pair : data) {
    compressed_data[index] = std::get<0>(pair);
    index++;
  }

  // Append the sensor values after the list of identifiers, in order
  for (const SensorVariantPair &pair : data) {
    std::visit(
      [&](auto v) {
        size_t size = sizeof(v);
        char* value = static_cast<char*>(static_cast<void*>(&v));
        for (size_t i = 0; i < size; i++) {
          compressed_data[index] = *(value + i);
          index++;
        }
      },
      std::get<1>(pair)
    );
  }

  return compressed_data;
}

std::vector<SensorVariantPair> VFDCPEncoder::decode_data(
  std::vector<char> data, 
  std::unordered_map<unsigned char, Sensor>& sensors
) {
  // Get the sensor ids
  size_t sensor_count = data[0];
  unsigned char sensor_ids[sensor_count];
  for (int i = 1; i < sensor_count + 1; i++) {
    sensor_ids[i - 1] = data[i];
  }

  // Decode the data
  int index = sensor_count + 1;
  std::vector<SensorVariantPair> decoded{};
  for (const unsigned char& sensor_id: sensor_ids) {
    auto variant = sensors[sensor_id].get_variant();
      std::visit(
        [&](auto v) {
          size_t size = sizeof(v);
          char decoded_bytes[size];
          for (size_t i = 0; i < size; i++) {
            decoded_bytes[i] = data[index];
            index++;
          }
          decltype(v) final_value = *static_cast<decltype(v)*>(static_cast<void*>(&decoded_bytes));
          decoded.emplace_back(sensor_id, final_value);
        }, 
        variant
      );
  }

  return decoded;
}