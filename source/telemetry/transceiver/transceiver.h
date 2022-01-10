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

class Transceiver {
  private:
    std::string _serial_number;
    std::string _connection_address = "https://srvelocity.iot.com";
    unsigned short _client_tcp_port;
    unsigned short _remote_udp_port;

  public:
    Transceiver(std::string sn) : _serial_number(sn) {}
    ~Transceiver();

    /**
     * @brief Fetch the entire list of sensors for the particular serial number of a "thing."
     */
    std::vector<Sensor> fetch_sensors();

    /**
     * @brief Fetch the sensors that have changed since the last update 
     */
    std::unordered_map<unsigned char, Sensor> fetch_sensor_diff(unsigned long long last_update);

    /**
     * @brief 
     */
    bool request_session();

    /**
     * @brief 
     */
    void send_vfdcp_data(std::vector<char>& bytes);
};