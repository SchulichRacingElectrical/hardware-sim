/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "sensor_coordinator.h"

SensorCoordinator::SensorCoordinator(unsigned long int s) {
  _serial_number = s;
}

void SensorCoordinator::_reconcile_sensors() {
  // First, we want to check memory - do we have sensors?
    // If we do have sensors in memory
      // Let's send a message to our server with our most recent update time
      // The server will respond with all the sensors that have been updated since the most recent update time
    // Else
      // Let's request all of the sensors from the server
  // Once we have a full sensor list, let's make sure the stored memory is updated. 
}