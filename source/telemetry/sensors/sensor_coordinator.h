/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "sensor.h"
#include <unordered_map>

class SensorCoordinator {
  private:
    unsigned long int _serial_number;
    std::unordered_map<unsigned char, std::shared_ptr<Sensor>> _sensor_map;

    void _reconcile_sensors();

  public:
    SensorCoordinator(unsigned long int s);
};