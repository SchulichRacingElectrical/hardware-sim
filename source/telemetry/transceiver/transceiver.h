/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../thing/sensor.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <sys/socket.h>
#include <httplib.h>
#include <json.hpp>

using json = nlohmann::json;

/**
 * @brief Presently incomplete class used for interacting with the server. Uses a combination
 * of REST API calls, TCP socket for direct messaging (e.g. sending data to a car for transmission),
 * UDP socket for sending compressed data for real-time. 
 */
class Transceiver {
  private:
    std::string _serial_number;
    httplib::Client _client;
    unsigned short _client_tcp_port = -1;

    int _sockfd;
    unsigned short _remote_udp_port = -1;
    std::string _remote_udp_address;
    struct sockaddr_in _server_address;

  public:
    Transceiver(std::string sn) : _serial_number(sn), _client("http://srvelocity.com") {
      _client.set_keep_alive(true);
    }
    ~Transceiver();

    /**
     * @brief Fetch the entire list of sensors for the particular serial number of a "thing."
     */
    std::vector<Sensor> fetch_sensors();

    /**
     * @brief Fetch the sensors that have changed since the last update 
     */
    std::unordered_map<unsigned char, Sensor> 
    fetch_sensor_diff(unsigned long long last_update);

    /**
     * @brief Requests the server to start a session, gets a UDP port to send to if successful. 
     * Returns true on success and false on failure. 
     */
    bool request_session();

    /**
     * @brief Opens the UDP socket through which compressed data is sent. 
     */
    void start_session();

    /**
     * @brief Sends compressed data to the server via UDP
     */
    void send_vfdcp_data(std::vector<unsigned char>& bytes);
};