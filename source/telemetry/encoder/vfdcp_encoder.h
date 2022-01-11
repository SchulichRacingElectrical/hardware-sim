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
    std::vector<unsigned char> encode_data(unsigned int timestamp, std::vector<SensorVariantPair>& data);
    
    /**
     * @brief 
     */
    std::tuple<unsigned int, std::vector<SensorVariantPair>> decode_data(
      std::vector<unsigned char> data, 
      std::unordered_map<unsigned char, Sensor>& sensors
    );
};