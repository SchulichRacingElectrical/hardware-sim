/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef TELEMETRY_THING_H
#define TELEMETRY_THING_H

#include "sensor.h"
#include <telemetry/transceiver/transceiver.h>
#include <telemetry/encoder/vfdcp_encoder.h>
#include <telemetry/stream/stream.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <ctime>

/**
 * @brief 
 */
class TelemetryThing {
  private:
    std::string _name;
    std::time_t _session_start_time;
    std::string _serial_number;
    std::unique_ptr<Stream> _data_stream;
    std::unique_ptr<Transceiver> _transceiver;
    std::vector<Sensor> _sensors;
    std::string _last_line; // Temporary for storing last line of file when writing

    /**
     * @brief Reconciles locally stored sensors and those on the cloud, updates 
     * locally stored sensors on change. Called on construction. 
     */
    void _populate_sensors();

    /**
     * @brief Function for logging what data is being sent and what it contains.
     */
    void _log_transmission(std::vector<unsigned char> bytes);

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
     * @brief Temporary friend that allows the CLI to display information about the Thing.
     */
    friend int main();
};

#endif