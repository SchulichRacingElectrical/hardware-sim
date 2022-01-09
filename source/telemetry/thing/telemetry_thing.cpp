/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "telemetry_thing.h"

TelemetryThing::TelemetryThing(std::string n, std::string sn) : _name(n), _serial_number(sn) {
  _transceiver = std::make_unique<Transceiver>(_serial_number);
  _populate_sensors();
}

void TelemetryThing::start_telemetry() {
  if (_data_stream == nullptr) {
    _data_stream = std::make_unique<Stream>(_sensors);
    _data_stream->subscribe(std::this_thread, &this->on_stream_update);
  } 
  _data_stream->open();
}

void TelemetryThing::stop_telemetry() {
  if (_data_stream != nullptr) {
    _data_stream->unsubscribe(std::this_thread::get_id());
    _data_stream->close();
  }
}

void TelemetryThing::pause_telemetry() {
  if (_data_stream != nullptr) {
    _data_stream->pause();
  }
}

void TelemetryThing::unpause_telemetry() {
  if (_data_stream != nullptr) {
    _data_stream->unpause();
  }
}

void TelemetryThing::on_stream_update(
  unsigned long long timestamp, 
  std::vector<SensorVariantPair> data
) {
  // Send the data to the transceiver
}

void TelemetryThing::_populate_sensors() {
  // Get the sensor data from the transceiver
  // Get the full set of data from the local system
  // If it does not exist, fetch all sensors
  // If it does exist, request sensor diff
}