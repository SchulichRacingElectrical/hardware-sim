/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "sensor.h"
#include "../stream/stream.h"
#include "../transceiver/transceiver.h"
#include "../encoder/vfdcp_encoder.h"
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <unordered_map>

class TelemetryThing {
  private:
    std::string _name;
    std::string _serial_number;
    std::unique_ptr<Stream> _data_stream;
    std::unique_ptr<Transceiver> _transceiver;
    std::vector<Sensor> _sensors;

    /**
     * @brief Reconciles locally stored sensors and those on the cloud, updates 
     * locally stored sensors on change. Called on construction. 
     */
    void _populate_sensors();

  public:
    TelemetryThing(std::string n, std::string sn);
    ~TelemetryThing();

    /**
     * @brief Starts the telemetry session. Does nothing if the session is already running.
     */
    void start_telemetry();

    /**
     * @brief Stops the telemetry session completely. Destroys the data stream.
     */
    void stop_telemetry();

    /**
     * @brief Pauses the telemetry session if it is running. This will stop the session 
     * on the server as the server session acts through a timeout. 
     */
    void pause_telemetry();

    /**
     * @brief Unpauses the telemetry session if it is paused. Does nothing if there is no
     * active session. This will start a new server session. 
     */
    void unpause_telemetry();

    /**
     * @brief Temporary friend that allows the CLI to display information about the Thing.
     */
    friend int main();
};