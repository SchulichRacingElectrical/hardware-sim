/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "transceiver.h"

Transceiver::~Transceiver() {
  // Close UDP port
  // Close TCP port
}

std::vector<Sensor> Transceiver::fetch_sensors() {
  // Make request to sensor endpoint
  std::vector<Sensor> sensors;
  return sensors;
}

std::unordered_map<unsigned char, Sensor> Transceiver::fetch_sensor_diff(unsigned long long last_update) {
  // Make request to get updated sensors
  std::unordered_map<unsigned char, Sensor> temp;
  return temp;
}

bool Transceiver::request_session() {
  // Request the server to start a new UDP sensor session
  // Use lib to make http request. 
  return false;
}

void Transceiver::send_vfdcp_data(std::vector<char>& bytes) {
  // Send data over udp
}