/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "sensor.h"
#include <iostream>

// There must be a better way to do this...
SensorDataVariant Sensor::get_variant() const {
    SensorDataVariant variant;
    switch (sensor_type) {
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