/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include <iostream>
#include <tuple>
#include <vector>
#include <variant>
#include <algorithm>
#include <cassert>

/**
 * @brief This represents all the possible data types our system
 *  can read. Based on delegation with the team, we found
 *  that these datatypes can represent all of our sensors.
 *  In the future, this list may be expanded.
 */
using SensorDataVariant =
  std::variant<
    long long, // q - char type representation from sensor
    double,    // d
    float,     // f
    int,       // i
    short,     // h
    char,      // c
    bool       // ?
  >;

/**
 * @brief A tuple (sensor ID, SensorVariantData)
 */
using SensorVariantPair = std::tuple<unsigned char, SensorDataVariant>;

/**
 * @brief An array of bytes containing our sensor ids
 * and compressed data along with a size for sending 
 * sensor data of UDP. 
 */
using CompressedData = std::tuple<char*, size_t>; 

/**
 * @brief Variable Frequency Data Compression Protocal Encoder
 * @details A singleton class that provides an interface for passing 
 *  a list of variants and outputs a byte array containing
 *  all information needed to decode the data on the server side. 
 */
class VFDCPEncoder {
  private:
    VFDCPEncoder() {}

  public:
    VFDCPEncoder(const VFDCPEncoder&) = delete;
    VFDCPEncoder &operator=(const VFDCPEncoder&) = delete;

    static VFDCPEncoder &get() {
      static VFDCPEncoder instance;
      return instance;
    }

    CompressedData encode_data(std::vector<SensorVariantPair>& data);
};