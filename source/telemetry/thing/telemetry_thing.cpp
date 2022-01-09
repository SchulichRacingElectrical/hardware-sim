/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "telemetry_thing.h"

TelemetryThing::TelemetryThing(std::string n, std::string sn) : _name(n), _serial_number(sn) {
  _transceiver = std::make_unique<Transceiver>(_serial_number);
  _populate_sensors();
}

void TelemetryThing::start_telemetry() {
  if (_data_stream == nullptr) {
    _data_stream = std::make_unique<Stream>(_sensors);
    _data_stream->subscribe(std::this_thread, &this->on_stream_update);
  } 
  _data_stream->open();
}

void TelemetryThing::stop_telemetry() {
  if (_data_stream != nullptr) {
    _data_stream->unsubscribe(std::this_thread::get_id());
    _data_stream->close();
  }
}

void TelemetryThing::pause_telemetry() {
  if (_data_stream != nullptr) {
    _data_stream->pause();
  }
}

void TelemetryThing::unpause_telemetry() {
  if (_data_stream != nullptr) {
    _data_stream->unpause();
  }
}

void TelemetryThing::on_stream_update(
  unsigned long long timestamp, 
  std::vector<SensorVariantPair> data
) {
  // Send the data to the transceiver
  std::vector<char> bytes = encode_data(data);
  _transceiver->send_vfdcp_data(bytes);
}

void TelemetryThing::_populate_sensors() {
  // Fetch the sensors (FUTURE: Check if the sensors are stored on disk first)
  _sensors = _transceiver->fetch_sensors();
  unsigned long int most_recent_update = 0;
  for (const auto& sensor: _sensors) {
    if (most_recent_update < sensor.last_update) {
      most_recent_update = sensor.last_update;
    }
  }

  // Renconcile any sensors that need to be updated
  std::unordered_map<unsigned char, Sensor> updated_sensors = _transceiver->fetch_sensor_diff(most_recent_update);
  for (auto sensor: _sensors) {
    if (updated_sensor.find(sensor.id) != updated_sensors.end()) {
      swap(sensor, updated_sensors[sensor.id]);
    }
  }

  // TODO: Store updated sensor list locally
}