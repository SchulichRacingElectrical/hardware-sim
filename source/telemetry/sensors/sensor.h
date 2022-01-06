/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include <any>
#include <typeindex>
#include <type_traits>
#include <string>

/**
 * TODO: We want to serialize the sensor to avoid storing json and also reduce data request size
 */

class Sensor: public AbstractSensor {
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
  decltype(auto) get_type();
  void serialize_and_store();
};