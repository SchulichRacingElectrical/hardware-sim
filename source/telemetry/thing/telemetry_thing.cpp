/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "telemetry_thing.h"
#include <store/thing_parser.h>
#include <store/thing_writer.h>

TelemetryThing::TelemetryThing(std::string n, std::string sn): _name(n), _serial_number(sn) {
  _transceiver = std::make_unique<Transceiver>(_serial_number);
  _populate_sensors();
}

TelemetryThing::~TelemetryThing() {
  _transceiver.reset();
  stop_telemetry();
}

void TelemetryThing::start_telemetry() {
  if (_data_stream == nullptr) {
    if (_transceiver->request_session()) {
      _transceiver->start_session();
      _session_start_time = std::time(nullptr);
      _data_stream = std::make_unique<Stream>(_sensors);
      unsigned int id = std::hash<std::thread::id>()(std::this_thread::get_id());
      auto callback = [&](unsigned int timestamp, std::vector<SensorVariantPair> data) {
        // Lock with mutex?
        std::vector<unsigned char> bytes = VFDCPEncoder::get().encode_data(timestamp, data);

        // Write data to file
        std::string path = "./storage/" + _serial_number + "_" + std::ctime(&_session_start_time) + "_data.txt";
        _last_line = ThingWriter::write_sensor_data(_sensors, data, timestamp, _last_line, path);

        // Send data to server
        _transceiver->send_vfdcp_data(bytes);

        // TEMPORARY - Log data 
        _log_transmission(bytes);
      };
      _data_stream->subscribe(id, callback);
    } else {
      // TODO: What if the session request fails?
    }
  } 
  _data_stream->open();
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

  // Find the most recent update time
  unsigned long int most_recent_update = 0;
  for (const auto& sensor: _sensors) {
    if (most_recent_update < sensor.traits["last_update"]) {
      most_recent_update = sensor.traits["last_update"];
    }
  }

  // Handle no sensors OR fetch potentially updated or new sensors
  if (_sensors.size() == 0) {
    std::cout << "Fetching sensors from server" << std::endl;
    _sensors = _transceiver->fetch_sensors();
  } else {
    std::cout << "Read sensors from disk, checking if there were any updates..." << std::endl;
    std::unordered_map<unsigned char, Sensor> sensor_diff = _transceiver->fetch_sensor_diff(most_recent_update);
    for (auto sensor: _sensors) {
      if (sensor_diff.find(sensor.traits["id"]) != sensor_diff.end()) {
        std::swap(sensor, sensor_diff[sensor.traits["id"]]);
        sensor_diff.erase(sensor.traits["id"]);
      }
    }
    for (auto new_sensor: sensor_diff) {
      _sensors.push_back(new_sensor.second);
    }
  }

  // After reconciling all sensors, write them to disk
  ThingWriter::write_sensors(_sensors, _serial_number);
}

void TelemetryThing::_log_transmission(std::vector<unsigned char> bytes) {
  std::cout << "<-" << _name << " sending " << bytes.size() << " bytes of compressed data: 0x";
  for (int i = 0; i < bytes.size(); i++) {
    std::cout << std::hex << (0xFF & bytes[i]);
  }
  std::cout << "\n";
  std::cout << std::dec;

  // Decode
  std::unordered_map<unsigned char, Sensor> sensor_map;
  for (auto sensor: _sensors)
  {
    sensor_map[(unsigned char)sensor.traits["id"]] = sensor;
  }
  auto [ts, uncompressed] = VFDCPEncoder::get().decode_data(bytes, sensor_map);
  std::cout << "->Server received data from " << _name << " with timestamp ";
  printf("%u", ts);
  std::cout << ":\n";
  for (const auto& value: uncompressed)
  {
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