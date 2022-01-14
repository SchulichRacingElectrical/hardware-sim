/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "transceiver.h"

#define TESTING 1

Transceiver::~Transceiver() {
  stop_session();
}

std::vector<Sensor> Transceiver::fetch_sensors() {
  std::vector<Sensor> sensors;
  #if TESTING == 0
  // Actual request - TODO
  std::string endpoint = "/iot/" + _serial_number + "/sensors";
  if (auto res = _client.Get(std::move(endpoint.c_str()))) {
    // TODO: Parse sensors, realistically want to send bytes over json. 
  } else {
    // There was an error
  }
  #else
  // For testing purposes
  json sensor = {
    {"name", "A"},
    {"id", 0},
    {"type", 'q'},
    {"last_update", 1},
    {"frequency", 2},
    {"channel_id", 1},
    {"u_calib", 100},
    {"l_calib", -100},
    {"u_bound", 100},
    {"l_bound", -100}
  };
  sensors.push_back(Sensor(sensor));
  sensor["name"] = "B"; 
  sensor["id"] = 1;
  sensor["type"] = 'd';
  sensor["frequency"] = 3;
  sensor["channel_id"] = 2;
  sensors.push_back(Sensor(sensor));
  sensor["name"] = "C"; 
  sensor["id"] = 2;
  sensor["type"] = 'f';
  sensor["frequency"] = 1;
  sensor["channel_id"] = 3;
  sensors.push_back(Sensor(sensor));
  sensor["name"] = "D"; 
  sensor["id"] = 3;
  sensor["type"] = 'i';
  sensor["frequency"] = 4;
  sensor["channel_id"] = 4;
  sensors.push_back(Sensor(sensor));
  sensor["name"] = "E"; 
  sensor["id"] = 4;
  sensor["type"] = 'h';
  sensor["frequency"] = 2;
  sensor["channel_id"] = 5;
  sensors.push_back(Sensor(sensor));
  sensor["name"] = "F"; 
  sensor["id"] = 5;
  sensor["type"] = 'c';
  sensor["frequency"] = 3;
  sensor["channel_id"] = 6;
  sensors.push_back(Sensor(sensor));
  sensor["name"] = "G"; 
  sensor["id"] = 6;
  sensor["type"] = '?';
  sensor["frequency"] = 1;
  sensor["channel_id"] = 7;
  sensors.push_back(Sensor(sensor));
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
    // TODO: Create a TCP socket and send the opened port to the server
    // so it knows where to send data. 
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
      // Failed to create... uhhh...
    }
    memset(&server_address, 0, sizeof(server_address));
    _server_address.sin_family = AF_INET;
    _server_address.sin_port = htons(_remote_udp_port);
    _server_address.sin_addr.s_addr = inet_addr(_remote_udp_address);
  #endif
}

void Transceiver::stop_session() {
  _client.set_keep_alive(false);
  close(_sockfd);
}

void Transceiver::send_vfdcp_data(std::vector<unsigned char>& bytes) {
  unsigned char buffer[bytes.size()];
  for (int i = 0; i < bytes.size(); i++) {
    buffer[i] = bytes[i];
  } 
  #if TESTING == 0
    sendto(_sockfd, (const unsigned char*)buffer, bytes.size(), MSG_CONFIRM,
            (const struct sockaddr *)&_server_address, sizeof(_server_address));
  #endif
}

