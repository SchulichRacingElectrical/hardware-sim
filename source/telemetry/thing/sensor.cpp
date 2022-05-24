/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "sensor.h"
#include <iostream>

// Nasty because it reads from memory
// Delete this after migrating to thing parser
Sensor::Sensor(std::vector<std::string> keys, std::vector<std::string> values) {
  json data{};
  for (int i = 0; i < keys.size(); i++) {
    std::string key = keys[i];
    if (key == "name") data[key] = values[i];
    else if (key == "smallId") data[key] = std::stoi(values[i]);
    else if (key == "type") data[key] = values[i];
    else if (key == "lastUpdate") data[key] = std::stoul(values[i]);
    else if (key == "frequency") data[key] = (unsigned char)std::stoul(values[i]);
    else if (key == "canId") data[key] = (unsigned int)std::stoul(values[i]);
    else if (key == "upperCalibration") data[key] = (unsigned int)std::stoul(values[i]);
    else if (key == "lowerCalibration") data[key] = (unsigned int)std::stoul(values[i]);
  }
  this->traits = data;
}

// There must be a better way to do this too...
SensorDataVariant Sensor::get_variant() const {
    SensorDataVariant variant;
    std::string type = traits["type"];
    switch (type[0]) {
      case int(SensorType::BOOL):
        variant = bool(false);
        return variant;
      case int(SensorType::BYTE):
        variant = char(0);
        return variant;
      case int(SensorType::UNSIGNED_BYTE):
        variant = (unsigned char)(0);
        return variant;
      case int(SensorType::SHORT):
        variant = short(0);
        return variant;
      case int(SensorType::UNSIGNED_SHORT):
        variant = (unsigned short)(0);
        return variant;
      case int(SensorType::INT):
        variant = int(0);
        return variant;
      case int(SensorType::UNSIGNED_INT):
        variant = (unsigned int)(0);
        return variant;
      case int(SensorType::FLOAT):
        variant = float(0);
        return variant;
      case int(SensorType::LONG_LONG):
        variant = (long long)(0);
        return variant;
      case int(SensorType::UNSIGNED_LONG_LONG):
        variant = (unsigned long long)(0);
        return variant;
      case int(SensorType::DOUBLE):
        variant = double(false);
        return variant;
      default:
        throw std::runtime_error("Sensor does not have a supported type");
      };
    return variant;
}