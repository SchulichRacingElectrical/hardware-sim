/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include <any>
#include <string>
#include <variant>
#include <exception>

/**
 * @brief This represents all the possible data types our system
 *  can read. Based on delegation with the team, we found
 *  that these datatypes can represent all of our sensors.
 *  In the future, this list may be expanded.
 */
using SensorDataVariant = std::variant<long long, double, float, int, short, char, bool>;

/**
 * TODO: We want to serialize the sensor to avoid storing json and also reduce data request size
 */

class Sensor {
private:
  std::string _name;
  char _type;
  unsigned long int _last_update;
  unsigned int _frequency;
  unsigned int _channel_id;
  double _upper_calibration;
  double _lower_calibration;
  double _min_value;
  double _max_value;

public:
  auto get_variant() const {
    SensorDataVariant variant;
    long long x = 0;
    switch (_type) {
      case 'q':
        variant = x;
        return variant;
      case 'd':
        variant = double(0);
        return variant;
      case 'f':
        variant = float(0);
        return variant;
      case 'i':
        variant = int(0);
        return variant;
      case 'h':
        variant = short(0);
        return variant;
      case 'c':
        variant = char(0);
        return variant;
      case '?':
        variant = bool(false);
        return variant;
      default:
        break;
      };
    throw std::runtime_error("Sensor does not have a supported type");
  }
};