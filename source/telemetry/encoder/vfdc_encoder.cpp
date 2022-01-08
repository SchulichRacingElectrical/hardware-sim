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
  for (const char& sensor_id: sensor_ids) {
    try {
      auto variant = sensors[sensor_id].get_variant();
      std::visit([&](auto v) { // Does this run async?
        size_t size = sizeof(v);
        char decoded_value[size];
        for (size_t i = 0; i < size; i++) {
          decoded_value[i] = *temp;
          temp++;
        }
        decltype(v) final_value = *static_cast<decltype(v)*>(static_cast<void*>(&decoded_value));
        decoded.emplace_back(sensor_id, final_value);
      }, variant);
    }
    catch (...)
    {
      throw std::runtime_error("Decoded sensor ID does not have a corresponding type.");
    }
  }

  return decoded;
}