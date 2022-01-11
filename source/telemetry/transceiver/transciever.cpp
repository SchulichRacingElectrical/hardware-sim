/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "transceiver.h"

#define TESTING 1

Transceiver::~Transceiver() {
  _client.set_keep_alive(false);
  close(_sockfd);
}

std::vector<Sensor> Transceiver::fetch_sensors() {
  std::vector<Sensor> sensors;
  #if TESTING == 0
    // Actual request - TODO
    std::string endpoint = "/iot/" + _serial_number + "/sensors";
    if (auto res = _client.Get(std::move(endpoint.c_str()))) {
      // TODO: Parse sensors
    } else {
      // There was an error
    }
  #else
    // For testing purposes
    sensors.push_back(Sensor("A", 0, 'q', 100, 7, 1, {-100, 100}, {-100, 100}));
    sensors.push_back(Sensor("B", 1, 'd', 100, 8, 2, {-100, 100}, {-100, 100}));
    sensors.push_back(Sensor("C", 2, 'f', 100, 3, 3, {-100, 100}, {-100, 100}));
    sensors.push_back(Sensor("D", 3, 'i', 100, 5, 4, {-100, 100}, {-100, 100}));
    sensors.push_back(Sensor("E", 4, 'h', 100, 2, 5, {-100, 100}, {-100, 100}));
    sensors.push_back(Sensor("F", 5, 'c', 100, 10, 6, {-100, 100}, {-100, 100}));
    sensors.push_back(Sensor("G", 6, '?', 100, 1, 7, {0, 1}, {0, 1}));
  #endif
  return sensors;
}

std::unordered_map<unsigned char, Sensor> Transceiver::fetch_sensor_diff(unsigned long long last_update) {
  std::unordered_map<unsigned char, Sensor> temp;
  #if TESTING == 0  
    std::string endpoint = "/iot/" + _serial_number + "/sensor_diff/" + std::to_string(lat_update);
    if (auto res = _client.Get(std::move(endpoint.c_str()))) {
      // TODO: Update the sensors
    } else {
      // TODO: There was an error
    }
  #endif
  return temp;
}

bool Transceiver::request_session() {
  #if TESTING == 0
    std::string endpoint = "/iot/" + _serial_number + "/start";
    auto res = _client.Get(std::move(endpoint.c_str()));
    if (auto res = _client.Get(std::move(endpoint.c_str()))) {
      json response = json::parse(res->body);
      _remote_udp_port = response["port"];
      _remote_udp_address = response["address"];
      return true;
    } else {
      // TODO: There was an error
      return false;
    }
  #endif
  return true;
}

void Transceiver::start_session() {
  #if TESTING == 0
    int sockfd;
    struct sockaddr_in server_address;
    if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      // Failed to create...
    }
    memset(&server_address, 0, sizeof(server_address));
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_remote_udp_port);
    _server_address.sin_addr.s_addr = inet_addr(_remote_udp_address);
  #endif
}

void Transceiver::send_vfdcp_data(std::vector<unsigned char>& bytes) {
  unsigned char buffer[bytes.size()];
  for (int i = 0; i < bytes.size(); i++) buffer[i] = bytes[i];
  #if TESTING == 0
    sendto(_sockfd, (const unsigned char*)buffer, bytes.size(), MSG_CONFIRM,
            (const struct sockaddr *)&_server_address, sizeof(_server_address));
  #endif
}