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
    else if (key == "type") data[key] = (char)std::stoi(values[i]);
    else if (key == "lastUpdate") data[key] = std::stoul(values[i]);
    else if (key == "frequency") data[key] = (unsigned char)std::stoul(values[i]);
    else if (key == "channelId") data[key] = (unsigned int)std::stoul(values[i]);
    else if (key == "upperCalibration") data[key] = (unsigned int)std::stoul(values[i]);
    else if (key == "lowerCalibration") data[key] = (unsigned int)std::stoul(values[i]);
  }
  this->traits = data;
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