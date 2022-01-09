/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../stream/stream.h"
#include "../stream/channel.h"
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
 * @brief A tuple (sensor ID, SensorVariantData)
 */
using SensorVariantPair = std::tuple<unsigned char, SensorDataVariant>;

enum class SensorType : char {
  LONGLONG = 'q',
  DOUBLE = 'd',
  FLOAT = 'f',
  INT = 'i',
  SHORT = 'h',
  CHAR = 'c',
  BOOL = '?'
};

struct SensorRange {
  double lower;
  double upper;
};

class Sensor {
public:
  std::string name;
  unsigned char id;
  SensorType _type;
  unsigned long int last_update;
  unsigned int frequency;
  unsigned int channel_id;
  SensorRange calibration;
  SensorRange bounds;

  Sensor() {}
  Sensor(const char *n, char t, unsigned long int lu, unsigned int f, unsigned int ci, SensorRange c, SensorRange b) 
    : name(n), _type((SensorType)t), last_update(lu), frequency(f), channel_id(ci), calibration(c), bounds(b) {}

  /**
   * @brief Returns a variant that can be used to deduce 
   * @return auto - A variant with the type set with 0
   */
  SensorDataVariant get_variant() const;
};