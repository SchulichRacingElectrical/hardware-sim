/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "sensor.h"
#include "../stream/stream.h"
#include "../transceiver/transceiver.h"
#include <string>
#include <vector>
#include <memory>

class TelemetryThing {
  private:
    std::string _name;
    std::string _serial_number;
    std::unique_ptr<Stream> _data_stream;
    std::unique_ptr<Transceiver> _transceiver;
    std::vector<Sensor> _sensors;

  public:
    TelemetryThing();
};