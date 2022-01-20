/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef VFDCP_ENCODER_H
#define VFDCP_ENCODER_H

#include <telemetry/thing/sensor.h>
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

    static VFDCPEncoder& get() {
      static VFDCPEncoder instance;
      return instance;
    }

    /**
     * @brief Encodes data into the format:
     * Sensor Count (1 Byte), Timestamp (4 bytes), Sensors IDs (Variable; Equal to Sensor Count), 
     * Sensor Values (Variable; A function of sensor type and Sensor Count)
     */
    std::vector<unsigned char> encode_data(unsigned int timestamp, std::vector<SensorVariantPair>& data);
    
    /**
     * @brief Decodes data from the format above. Returns a tuple containing the data's 
     * timestamp and all of the data. 
     */
    std::tuple<unsigned int, std::vector<SensorVariantPair>> decode_data(
      std::vector<unsigned char>& data, 
      std::unordered_map<unsigned char, Sensor>& sensors
    );
};

#endif