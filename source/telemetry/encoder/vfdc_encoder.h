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
 * @brief Variable Frequency Data Compression Protocal Encoder.
 * A singleton class that provides an interface for passing 
 * a list of variants and outputs a byte array containing
 * all information needed to decode the data on the server side. 
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

    /**
     * @brief 
     */
    std::vector<char> encode_data(std::vector<SensorVariantPair>& data);
    
    /**
     * @brief 
     */
    std::vector<SensorVariantPair> decode_data(
      std::vector<char> data, 
      std::unordered_map<unsigned char, Sensor>& sensors
    );
};