/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "vfdcp_encoder.h"

std::vector<unsigned char> VFDCPEncoder::encode_data(
  unsigned int timestamp, 
  std::vector<SensorVariantPair>& data
) {
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
  // size = 1 + (timestamp size) + number of sensor ids + sum of data type sizes for current data + padding ([0, 3])
  size_t size = 0;
  for (const SensorVariantPair& pair : data) {
    size_t current_size;
    std::visit([&](auto v) { current_size = sizeof(v); size += current_size; }, std::get<1>(pair));
  }
  if (!(size % 4)) size += size % 4;
  size += size_t(data.size()) + 5; // 5 because 1 byte for size byte, 4 bytes for timestamp

  // First, let's push the number of sensor values we expect
  int index = 0;
  std::vector<unsigned char> compressed_data(size, 0);
  compressed_data[index] = size_t(data.size());
  index++;

  // Then, we can insert the timestamp
  unsigned char* timestamp_bytes = reinterpret_cast<unsigned char*>(&timestamp);
  for (size_t i = 0; i < 4; i++) {
    compressed_data[index] = *(timestamp_bytes + i);
    index++;
  }

  // Append the sensor ids after the timestamp
  for (const SensorVariantPair &pair : data) {
    compressed_data[index] = std::get<0>(pair);
    index++;
  }

  // Append the sensor values after the list of identifiers, in order
  for (const SensorVariantPair &pair : data) {
    std::visit(
      [&](auto v) {
        size_t size = sizeof(v);
        unsigned char* value = reinterpret_cast<unsigned char*>(&v);
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

std::tuple<unsigned int, std::vector<SensorVariantPair>> VFDCPEncoder::decode_data(
  std::vector<unsigned char> data, 
  std::unordered_map<unsigned char, Sensor>& sensors
) {
  // Get the sensor ids and current timestamp
  size_t sensor_count = data[0];
  unsigned int timestamp = *(unsigned int *)(&data[1]);
  unsigned char sensor_ids[sensor_count];
  for (int i = 5; i < sensor_count + 5; i++) {
    sensor_ids[i - 5] = data[i];
  }

  // Decode the data
  int index = sensor_count + 5;
  std::vector<SensorVariantPair> decoded{};
  for (const unsigned char& sensor_id: sensor_ids) {
    auto variant = sensors[sensor_id].get_variant();
      std::visit(
        [&](auto v) {
          size_t size = sizeof(v);
          unsigned char decoded_bytes[size];
          for (size_t i = 0; i < size; i++) {
            decoded_bytes[i] = data[index];
            index++;
          }
          decltype(v) final_value = *reinterpret_cast<decltype(v)*>(&decoded_bytes);
          decoded.emplace_back(sensor_id, final_value);
        }, 
        variant
      );
  }

  return { timestamp, decoded };
}