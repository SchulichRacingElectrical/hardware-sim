/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include "../thing/sensor.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <optional>
#include <httplib.h>
#include <json.hpp>
#include <arpa/inet.h>
#include <unistd.h>

#ifdef _WIN32
#include "winsock2.h"
#else
#include <sys/socket.h>
#endif

using json = nlohmann::json;
using SensorDiff = std::optional<std::pair<std::unordered_map<unsigned char, Sensor>, std::vector<std::string>>>;

/**
 * @brief Presently incomplete class used for interacting with the server. Uses a combination
 * of REST API calls, TCP socket for direct messaging (e.g. sending data to a car for transmission),
 * UDP socket for sending compressed data for real-time. 
 */
class Transceiver {
  private:
    std::string _api_key;

    // REST request fields
    std::string _serial_number;
    std::string _web_address = "http://199.116.235.51:8080";
    unsigned short _client_tcp_port = -1;

    // UDP socket fields
    int _sockfd;
    unsigned short _remote_udp_port = -1;
    std::string _remote_udp_address;
    struct sockaddr_in _server_address;

    // TODO: TCP socket fields

  public:
    Transceiver(std::string sn, std::string key) : _serial_number(sn), _api_key(key) {}
    ~Transceiver();

    /**
     * @brief Fetch the entire list of sensors for the particular serial number of a "thing."
     */
    std::optional<std::vector<Sensor>> fetch_sensors();

    /**
     * @brief Fetch the sensors that have changed since the last update 
     */
    SensorDiff fetch_sensor_diff(unsigned long long last_update);

    /**
     * @brief Requests the server to start a session, gets a UDP port to send to if successful. 
     * Returns true on success and false on failure. 
     */
    bool request_session();

    /**
     * @brief Opens the UDP socket through which compressed data is sent. 
     */
    bool initialize_udp();

    /**
     * @brief Closes the UDP socket " " " "
     */
    void stop_session();

    /**
     * @brief Sends compressed data to the server via UDP
     */
    void send_vfdcp_data(std::vector<unsigned char>& bytes);
};

#endif