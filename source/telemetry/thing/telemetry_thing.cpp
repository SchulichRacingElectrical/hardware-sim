/*
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#include "telemetry_thing.h"
#include <store/thing_parser.h>
#include <store/thing_writer.h>

TelemetryThing::TelemetryThing(std::string n, std::string sn, std::string k) : _name(n), _serial_number(sn), _api_key(k) {
  _transceiver = std::make_unique<Transceiver>(_serial_number, _api_key);
  _populate_sensors();
}

TelemetryThing::~TelemetryThing() {
  _transceiver.reset();
  stop_telemetry();
}

bool TelemetryThing::start_telemetry() {
  if (_data_stream == nullptr) {
    if (!this->_consolidate_sensors()) {
      std::cout << RED << "Failed to fetch sensors for Thing " << this->_name << ", telemetry will not begin for this Thing." << RESET << std::endl;
      return false;
    }
    if (this->_sensors.size() == 0) {
      std::cout << RED << "Thing " << this->_name << " has no sensors, telemetry will not begin for this Thing." << RESET << std::endl;
      return false;
    }
    if (_transceiver->request_session()) {
      _transceiver->initialize_udp();
      _session_start_time = std::time(nullptr);
      _data_stream = std::make_unique<Stream>(_sensors);
      unsigned int id = std::hash<std::thread::id>()(std::this_thread::get_id());
      auto callback = [&](unsigned int timestamp, std::vector<SensorVariantPair> data) {
        std::vector<unsigned char> bytes = VFDCPEncoder::get().encode_data(timestamp, data);
        // std::string path = "./storage/" + _serial_number + "_" + std::to_string(_session_start_time) + "_data.txt";
        // _last_line = ThingWriter::write_sensor_data(_sensors, data, timestamp, _last_line, path);
        _transceiver->send_vfdcp_data(bytes);
      };
      _data_stream->subscribe(id, callback);
    } else {
      std::cout << RED << "Failed to create session for Thing " << _name << "." << RESET << std::endl;
      return false;
    }
  }
  _data_stream->open();
  return true;
}

void TelemetryThing::stop_telemetry() {
  if (_data_stream != nullptr) {
    _transceiver->stop_session();
    unsigned int id = std::hash<std::thread::id>()(std::this_thread::get_id());
    _data_stream->unsubscribe(id);
    _data_stream.reset();
    _last_line = "";
  }
}

void TelemetryThing::_populate_sensors() {
  // Attempt to read the sensors from disk
  _sensors = ThingParser::read_sensors(_serial_number);
}

bool TelemetryThing::_consolidate_sensors() {
  // Find the most recent update time
  unsigned long int most_recent_update = 0;
  for (const auto &sensor : _sensors)
    if (most_recent_update < sensor.traits["lastUpdate"]) 
      most_recent_update = sensor.traits["lastUpdate"];

  // Handle no sensors OR fetch potentially updated or new sensors
  bool success = true;
  if (_sensors.size() == 0) {
    std::optional<std::vector<Sensor>> response = _transceiver->fetch_sensors();
    if (response.has_value()) {
      _sensors = response.value();
    } else {
      success = false;
    }
  } else {
    std::optional<std::unordered_map<unsigned char, Sensor>> response = _transceiver->fetch_sensor_diff(most_recent_update);
    if (response.has_value()) {
      std::unordered_map<unsigned char, Sensor> sensor_diff = response.value();
      for (auto sensor : _sensors) {
        if (sensor_diff.find(sensor.traits["smallId"]) != sensor_diff.end()) {
          std::swap(sensor, sensor_diff[sensor.traits["smallId"]]);
          sensor_diff.erase(sensor.traits["smallId"]);
        }
      }
      for (auto new_sensor : sensor_diff)
        _sensors.push_back(new_sensor.second);
    } else {
      success = false;
    }
  }

  // After reconciling all sensors, write them to disk
  ThingWriter::write_sensors(_sensors, _serial_number); 
  return success;
}

void TelemetryThing::_log_transmission(std::vector<unsigned char> bytes) {
  std::cout << "<-" << _name << " sending " << bytes.size() << " bytes of compressed data: 0x";
  for (int i = 0; i < bytes.size(); i++)
    std::cout << std::hex << (0xFF & bytes[i]);
  std::cout << "\n";
  std::cout << std::dec;

  // Decode
  std::unordered_map<unsigned char, Sensor> sensor_map;
  for (auto sensor : _sensors)
    sensor_map[(unsigned char)sensor.traits["smallId"]] = sensor;
  auto [ts, uncompressed] = VFDCPEncoder::get().decode_data(bytes, sensor_map);
  std::cout << "->Server received data from " << _name << " with timestamp ";
  printf("%u", ts);
  std::cout << ":\n";
  for (const auto &value : uncompressed) {
    auto sensor_id = std::get<0>(value);
    std::visit(
      [&](auto v) {
        std::cout << "Sensor with id " << int(sensor_id) << " has value " << int(v) << " of type " << typeid(v).name() << std::endl;
      },
      std::get<1>(value)
    );
  }
  std::cout << std::endl;
}