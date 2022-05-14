/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "transceiver.h"

Transceiver::~Transceiver() {
  stop_session();
}

std::optional<std::vector<Sensor>> Transceiver::fetch_sensors() {
  std::vector<Sensor> sensors;
  std::string endpoint = "/api/database/sensors/thing/" + _serial_number;
  httplib::Client client(this->_web_address);
  client.set_read_timeout(100000);
  httplib::Headers headers = {{"apiKey", _api_key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      json body = json::parse(res->body).at("data");
      for (json::iterator it = body.begin(); it != body.end(); ++it) {
        sensors.push_back(Sensor(*it));
      }
    } else {
      return std::nullopt;
    }
  } else {
    return std::nullopt;
  }
  return sensors;
}

std::optional<std::unordered_map<unsigned char, Sensor>> Transceiver::fetch_sensor_diff(unsigned long long last_update) {
  std::unordered_map<unsigned char, Sensor> sensor_map;
  std::string endpoint = "/api/database/sensors/thing/" + _serial_number + "/lastUpdate/" + std::to_string(last_update);
  httplib::Client client(this->_web_address);
  client.set_read_timeout(100000);
  httplib::Headers headers = {{"apiKey", _api_key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      json body = json::parse(res->body).at("data");
      for (json::iterator it = body.begin(); it != body.end(); ++it) {
        json element = *it; // What if this fails?
        unsigned char small_id = element.at("smallId");
        sensor_map[small_id] = Sensor(*it);
      }
    } else {
      return std::nullopt;
    }
  } else {
    return std::nullopt;
  }
  return sensor_map;
}

bool Transceiver::request_session() {
  std::string endpoint = "/api/iot/" + _serial_number + "/start";
  httplib::Client client(this->_web_address);
  httplib::Headers headers = {{"apiKey", _api_key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      json response = json::parse(res->body);
      _remote_udp_port = response.at("port");
      _remote_udp_address = response.at("address");
      // TODO: Create a tcp port in the future to connect to the server
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool Transceiver::initialize_udp() {
  int sockfd;
  struct sockaddr_in server_address;
  if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return false;
  }
  memset(&server_address, 0, sizeof(server_address));
  _server_address.sin_family = AF_INET;
  _server_address.sin_port = htons(_remote_udp_port);
  _server_address.sin_addr.s_addr = inet_addr(_remote_udp_address.c_str());
  return true;
}

void Transceiver::stop_session() {
  int status = close(_sockfd);
  // Do something with this status!
}

void Transceiver::send_vfdcp_data(std::vector<unsigned char>& bytes) {
  unsigned char buffer[bytes.size()];
  for (int i = 0; i < bytes.size(); i++) buffer[i] = bytes[i];
  sendto(
    _sockfd, 
    (const unsigned char*)buffer, 
    bytes.size(), 
    0, // TODO: Not sure what this should be
    (const struct sockaddr *)&_server_address, 
    sizeof(_server_address)
  );
}

