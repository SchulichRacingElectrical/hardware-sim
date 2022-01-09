/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../thing/sensor.h"
#include <tuple>
#include <vector>
#include <variant>
#include <algorithm>
#include <unordered_map>

/**
 * @brief An array of bytes containing our sensor ids
 *  and compressed data along with a size for sending 
 *  sensor data of UDP. 
 */
using CompressedData = std::tuple<std::vector<char>, size_t>; 

/**
 * @brief Variable Frequency Data Compression Protocal Encoder
 * @details A singleton class that provides an interface for passing 
 *  a list of variants and outputs a byte array containing
 *  all information needed to decode the data on the server side. 
 */
class VFDCPEncoder {
  VFDCPEncoder() {}

  public:
    VFDCPEncoder(const VFDCPEncoder&) = delete;
    VFDCPEncoder &operator=(const VFDCPEncoder&) = delete;

    static VFDCPEncoder &get() {
      static VFDCPEncoder instance;
      return instance;
    }

    CompressedData encode_data(std::vector<SensorVariantPair>& data);
    std::vector<SensorVariantPair> decode_data(
      std::vector<char> data, 
      std::unordered_map<unsigned char, Sensor>& sensors
    );
};