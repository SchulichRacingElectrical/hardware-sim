/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "sensor.h"
#include <iostream>

// There must be a better way to do this...
// Minor optimization is to move vectors rather than copy them
Sensor::Sensor(std::vector<std::string> keys, std::vector<std::string> values) {
  json data{};
  std::set<std::string> doubles{"u_calib", "l_calib", "u_bound", "l_bound"};
  for (int i = 0; i < keys.size(); i++) {
    std::string key = keys[i];
    if (key == "name") data[key] = values[i];
    else if (key == "id") data[key] = std::stoi(values[i]);
    else if (key == "type") data[key] = (char)std::stoi(values[i]);
    else if (key == "last_update") data[key] = std::stoul(values[i]);
    else if (key == "frequency") data[key] = (unsigned char)std::stoul(values[i]);
    else if (key == "channel_id") data[key] = (unsigned int)std::stoul(values[i]);
    else if (doubles.find(key) != doubles.end()) data[key] = std::stod(values[i]);
  }
  traits = data;
}

// There must be a better way to do this too...
SensorDataVariant Sensor::get_variant() const {
    SensorDataVariant variant;
    switch (((SensorType)traits["type"])) {
      case SensorType::LONGLONG:
        variant = (long long)(0);
        return variant;
      case SensorType::DOUBLE:
        variant = double(0);
        return variant;
      case SensorType::FLOAT:
        variant = float(0);
        return variant;
      case SensorType::INT:
        variant = int(0);
        return variant;
      case SensorType::SHORT:
        variant = short(0);
        return variant;
      case SensorType::CHAR:
        variant = char(0);
        return variant;
      case SensorType::BOOL:
        variant = bool(false);
        return variant;
      default:
        throw std::runtime_error("Sensor does not have a supported type");
      };
    return variant;
}