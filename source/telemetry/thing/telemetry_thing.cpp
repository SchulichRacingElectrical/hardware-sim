/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "telemetry_thing.h"

void TelemetryThing::start_telemetry() {

}

void TelemetryThing::pause_telemetry() {

}

void TelemetryThing::stop_telemetry() {

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