/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
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

/**
 * @brief Enumeration used to deduce sensor type from the database. May be expanded but 
 * the expansion must be added to SensorDataVariant. 
 */
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

/**
 * @brief Effectively a struct. Represents sensors on a "Thing"; used to create channels to 
 * read data from. These sensors are stored on a remote database and are fetched when a 
 * "Thing" is created. 
 */
class Sensor {
public:
  /**
   * @brief Name of the sensor (e.g. Potentiometer)
   */
  std::string name;

  /**
   * @brief The sensor id used to encode/decode data. Limited to 255 to keep data for
   * low sensor count "Things" small. This software is not expected to be used with 
   * "Things" that have more than 255 sensors. 
   */
  unsigned char id;

  /**
   * @brief An enumeration representing the sensor's data type.
   */
  SensorType sensor_type;

  /**
   * @brief Used to determine whether the sensor has been updated more recently
   * against the database. 
   */
  unsigned long int last_update;

  /**
   * @brief The update frequency of the sensor in Hz. Must be between 1-255, the 
   * system performs best when all sensor frequencies are divisible by each other. 
   */
  unsigned char frequency;

  /**
   * @brief A channel ID that is used when created a "channel" for a piece of hardware. 
   * Real-world channel examples include CAN, SPI, GPIO, I2C, UART, etc. 
   */
  unsigned int channel_id; 

  /**
   * @brief Some sensors output raw voltages, but are not calibrated (e.g. A sensor that
   * is expected to output 0-5V actually outputs 0.2-4.8V). This value is used to scale
   * values to normalize into their appropriate range. 
   */
  SensorRange calibration;

  /**
   * @brief Upper and lower most sensor values. Must fit within the sensor's type. 
   */
  SensorRange bounds;

  Sensor() {}
  Sensor(const char *n, unsigned char i, char t, unsigned long int lu, unsigned char f, unsigned int ci, SensorRange c, SensorRange b) 
    : name(n), id(i), sensor_type((SensorType)t), last_update(lu), frequency(f), channel_id(ci), calibration(c), bounds(b) {}

  /**
   * @brief Returns a variant that can be used to deduce the sensor's data type.
   */
  SensorDataVariant get_variant() const;
};