/*
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#ifndef TELEMETRY_THING_H
#define TELEMETRY_THING_H

#include "sensor.h"
#include <terminal_consts.h>
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
 * @brief A telemetry "thing" is an object that contains a set of sensors that write
 * data through a stream. The thing is responsible for mediating data between the
 * stream, file store, and data transceiver.
 */
class TelemetryThing
{
private:
  std::string _name;
  std::time_t _session_start_time;
  std::string _serial_number;
  std::string _api_key;
  std::unique_ptr<Stream> _data_stream;
  std::unique_ptr<Transceiver> _transceiver;
  std::vector<Sensor> _sensors;
  std::string _last_line; // Temporary for storing last line of file when writing

  /**
   * @brief Reads sensor data from disk.
   */
  void _populate_sensors();

  /**
   * @brief Reconciles locally stored sensors and those on the cloud, updates
   * locally stored sensors on change. Called on telemetry start.
   */
  bool _consolidate_sensors();

  /**
   * @brief Function for logging what data is being sent and what it contains.
   */
  void _log_transmission(std::vector<unsigned char> bytes);

public:
  TelemetryThing(std::string n, std::string sn, std::string k);
  ~TelemetryThing();

  /**
   * @brief Starts the telemetry session. Does nothing if the session is already running.
   */
  void start_telemetry();

  /**
   * @brief Stops the telemetry session completely. Destroys the data stream.
   */
  void stop_telemetry();

  friend auto operator<<(std::ostream& os, TelemetryThing const& thing) -> std::ostream& { 
    return os << thing._serial_number << ", " << thing._name;
  }
};

#endif