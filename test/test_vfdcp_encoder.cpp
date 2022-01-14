/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "gtest/gtest.h"
#include <telemetry/thing/sensor.h>
#include <telemetry/encoder/vfdcp_encoder.h>
#include <cassert>
#include <bitset>
#include <iostream>
#include <unordered_map>

// Fixture for sensors
class VFDCPEncoderTests: public ::testing::Test {
public:
  static std::unordered_map<unsigned char, Sensor> sensor_map;

protected:
  static void SetUpTestSuite() {
    // TODO: Use JSON
    // sensor_map[0] = Sensor("A", 0, 'q', 100, 2, 1, {-100, 100}, {-100, 100});
    // sensor_map[1] = Sensor("B", 1, 'd', 100, 3, 2, {-100, 100}, {-100, 100});
    // sensor_map[2] = Sensor("C", 2, 'f', 100, 1, 3, {-100, 100}, {-100, 100});
    // sensor_map[3] = Sensor("D", 3, 'i', 100, 4, 4, {-100, 100}, {-100, 100});
    // sensor_map[4] = Sensor("E", 4, 'h', 100, 2, 5, {-100, 100}, {-100, 100});
    // sensor_map[5] = Sensor("F", 5, 'c', 100, 3, 6, {-100, 100}, {-100, 100});
    // sensor_map[6] = Sensor("G", 6, '?', 100, 1, 7, {0, 1}, {0, 1});
  }

  static void TearDownTestSuite() {
    sensor_map.clear();
  }

  void SetUp() override {}
  void TearDown() override {}
};

std::unordered_map<unsigned char, Sensor> VFDCPEncoderTests::sensor_map{};

TEST_F(VFDCPEncoderTests, TestSingleEncode) {
  unsigned char id = 1;
  SensorDataVariant datum = double(1000);
  SensorVariantPair pair = {id, datum};
  std::vector<SensorVariantPair> data;
  data.push_back(pair);
  std::vector<unsigned char> compressed = VFDCPEncoder::get().encode_data(254, data);

  // Ensure our size is 10
  EXPECT_EQ(compressed.size(), 10);

  // Ensure our sensor count is 1
  EXPECT_EQ(compressed[0], 1);

  // Ensure the first byte is the char id = 1
  unsigned int timestamp = *(unsigned int *)(&compressed[1]);
  EXPECT_EQ(timestamp, 254);

  // Ensure the first value is 1000
  double value = *(double *)(&compressed[5]);
  EXPECT_EQ(value, 1000);
}

TEST_F(VFDCPEncoderTests, TestMultiEncode) {
  std::vector<SensorVariantPair> data;
  long long first = 30;
  data.push_back({0, first}); // 8
  data.push_back({1, double(15)}); // 8
  data.push_back({2, float(10)}); // 4
  data.push_back({3, int(4)}); // 4
  data.push_back({4, short(3)}); // 2
  data.push_back({5, char(97)}); // 1
  data.push_back({6, bool(false)}); // 1
  std::vector<unsigned char> compressed = VFDCPEncoder::get().encode_data(3, data);
  
  // Ensure our size is 7
  EXPECT_EQ(compressed[0], 7);

  // Ensure the byte size is correct: 1 + 4 + 7 + 8 + 8 + 4 + 4 + 2 + 1 + 1 = 40
  EXPECT_EQ(compressed.size(), 40);

  // Let's decompress the data
  auto[timestamp, uncompressed] = VFDCPEncoder::get().decode_data(compressed, VFDCPEncoderTests::sensor_map);

  // The timestamp should be 3
  EXPECT_EQ(timestamp, 3);

  // Data sizes should be the same
  EXPECT_EQ(data.size(), uncompressed.size());

  // Data values should be the same
  for (int i = 0; i < uncompressed.size(); i++) {
    std::visit(
      [&](auto v) {
        std::visit(
          [&](auto original)
          { 
            EXPECT_EQ(v, original); 
        }, std::get<1>(data[i]));
      },
      std::get<1>(uncompressed[i]));
  }
}

TEST_F(VFDCPEncoderTests, TestByteArrangement) {
  // TODO: Test if sensor data input in order of smallest to largest will
  // reorganize from largest to smallest. 
}

TEST_F(VFDCPEncoderTests, TestEmpty) {
  // TODO: Test what happens when we pass empty data
}

TEST_F(VFDCPEncoderTests, TestTimestampRange) {
  // TODO: Test if the timestamp ever gets interpreted negative
}

// TODO: Find other test cases and implement