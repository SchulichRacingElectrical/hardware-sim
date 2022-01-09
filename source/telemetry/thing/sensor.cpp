/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "sensor.h"

// This is pretty yikes
// See if it's possible to create a type from a type name (char/string)
// This will be platform specific though...
// Cannot use std::any since we cannot get the size. 
// Could use a sensor template factory but that still sucks. 
// SensorDataVariant Sensor::get_variant() const {
//     SensorDataVariant variant;
//     switch (_type) {
//       case SensorType::LONGLONG:
//         variant = (long long)(0);
//         return variant;
//       case SensorType::DOUBLE:
//         variant = double(0);
//         return variant;
//       case SensorType::FLOAT:
//         variant = float(0);
//         return variant;
//       case SensorType::INT:
//         variant = int(0);
//         return variant;
//       case SensorType::SHORT:
//         variant = short(0);
//         return variant;
//       case SensorType::CHAR:
//         variant = char(0);
//         return variant;
//       case SensorType::BOOL:
//         variant = bool(false);
//       default:
//         throw std::runtime_error("Sensor does not have a supported type");
//       };
//     return variant;
// }