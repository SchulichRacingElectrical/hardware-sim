/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef THING_WRITER_H
#define THING_WRITER_H

#include <telemetry/thing/sensor.h>
#include "thing_parser.h"
#include <unordered_map>
#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <vector>
#include <string>
//#include <cstdio>

using json = nlohmann::json;

/**
 * @brief A collection holding static class methods for writing/storing sensor data for a thing
 * along with telemetry session data. This class will be changed to be used as an intance by a
 * thing in the future. 
 */
class ThingWriter {
  public:
    ThingWriter() = delete;

    /**
     * @brief Writes a sensor collection to a file for storage. Will overwrite a previously
     * existing sensor file. Will name the file "SerialNumber_sensors.txt"
     */
    static void write_sensors(
      const std::vector<Sensor>& sensors, 
      const std::string serial_number
    );

    /**
     * @brief Writes new sensor data to a file. The method will read the previous line of the file
     * and populate non-updated sensors with their previous value in a new row. If a file does not
     * yet exist, it will name the file "SerialNumber_Date_data.txt". This function will soon be 
     * changed as constantly opening and closing ofstreams is bad. 
     */
    static std::string write_sensor_data(
      const std::vector<Sensor> &sensors,
      const std::vector<SensorVariantPair>& data,
      const unsigned int& timestamp,
      const std::string& last_line,
      const std::string& path
    );

    /**
     * @brief For internal use. Parses JSON into a comma separated string. Using the keys flag as true
     * will return a string with all the keys separated by commas. Keys false will return a string 
     * with all the values separated by commas. 
     */
    static std::string create_sensor_row(const json& traits, bool keys = false);
};

// Must define functions in header or we will get a linking error
// TODO: Update CMake to resolve linking errors

void ThingWriter::write_sensors(
  const std::vector<Sensor>& sensors, 
  const std::string serial_number
) {
  if (sensors.size() > 0) {
    // Delete the existing file
    std::string path = "./storage/" + serial_number + "_sensors.txt";
    std::remove(path.c_str());  

    // Create a new file and write to the first row (column names)
    std::ofstream out_file(path);
    std::string columnNames = ThingWriter::create_sensor_row(sensors[0].traits, true);
    out_file << columnNames << std::endl;

    // Write all of the sensors
    for (const Sensor& sensor: sensors) {
      std::string row = ThingWriter::create_sensor_row(sensor.traits);
      out_file << row << std::endl;
    }

    out_file.close();
  }
}

std::string ThingWriter::write_sensor_data(
  const std::vector<Sensor> &sensors,
  const std::vector<SensorVariantPair>& data,
  const unsigned int& timestamp,
  const std::string& last_line,
  const std::string& path
) {
  // Create a vector to determine the order of the columns
  std::vector<std::string> column_names;
  column_names.push_back("Timestamp");
  for (const Sensor &sensor : sensors) {
    column_names.push_back(sensor.traits["name"]);
  }

  // Create a sensor map
  std::unordered_map<unsigned char, Sensor> sensor_map;
  for (Sensor sensor: sensors) {
    sensor_map[sensor.traits["id"]] = sensor;
  }

  // Create a data map
  std::unordered_map<std::string, SensorDataVariant> data_map;
  for (SensorVariantPair datum: data) {
    data_map[sensor_map[std::get<0>(datum)].traits["name"]] = std::get<1>(datum);
  }

  std::string data_row;
  if (std::filesystem::exists(path) == false) {
    // Create the file and write the first column
    std::ofstream out_file(path);
    std::string column_name_row;
    for (int i = 0; i < column_names.size(); i++) {
      column_name_row += column_names[i];
      if (i != column_names.size() - 1) {
        column_name_row += ",";
      }
    }
    out_file << column_name_row << std::endl;

    // Create the first data string
    data_row += std::to_string(timestamp) + ",";
    for (int i = 1; i < column_names.size(); i++) {
      if (data_map.find(column_names[i]) != data_map.end()) {
        std::visit(
          [&](auto v) {
            data_row += std::to_string(v) + ",";
          },
          data_map[column_names[i]]);
      } else {
        data_row += "0,";
      }
    }
    data_row.pop_back();
    out_file << data_row << std::endl;
    out_file.close();
  } else {
    // Write to the last line of the file, populate missing columns with previous values
    std::vector<std::string> last_row = ThingParser::parse_row(last_line);
    std::ofstream out_file(path, std::ios::app);
    data_row += std::to_string(timestamp) + ",";
    for (int i = 1; i < column_names.size(); i++) {
      if (data_map.find(column_names[i]) != data_map.end()) {
        std::visit(
          [&](auto v) {
            data_row += std::to_string(v) + ",";
          },
          data_map[column_names[i]]
        );
      } else {
        data_row += last_row[i] + ",";
      }
    }
    data_row.pop_back();
    out_file << data_row << std::endl;
    out_file.close();
  }
  return data_row;
}

std::string ThingWriter::create_sensor_row(const json& traits, bool keys) {
  std::string row;
  for (const auto& element : traits.items()) {
    if (keys) {
      row += element.key() + ",";
    } else {
      auto value = element.value();
      row += (value.is_string() ? std::string(value) : to_string(value)) + ",";
    }
  }
  row.pop_back();
  return row;
}

#endif