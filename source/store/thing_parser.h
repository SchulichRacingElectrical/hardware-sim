/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef THING_PARSER_H
#define THING_PARSER_H

#include <telemetry/thing/sensor.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

/**
 * @brief A class with static methods that allows a thing to read its sensors and data.
 * This class will be changed in the near future to be instanced by a telemetry thing. 
 */
class ThingParser {
  public:
    ThingParser() = delete;

    /**
     * @brief Read and return all sensors for a thing.
     */
    static std::vector<Sensor> read_sensors(
      const std::string& serial_number
    );

    /**
     * @brief Read all data for a particular file for a particular sensor.
     * There are no gaps in data between timestamps. Reading different sensor
     * data within the same file will result in the same vector size.
     */
    static std::vector<SensorDataVariant> read_sensor_data(
      const std::string& file_name, 
      const Sensor& sensor
    );

    /**
     * @brief Returns a list of data filenames for a particular thing.
     */
    static std::vector<std::string> find_thing_data_file_names(
      const std::string& serial_number
    );

    /**
     * @brief Given a string row from a text file, this function will split the
     * string by a comma delimiter and return a vector of column values.
     */
    static std::vector<std::string> parse_row(const std::string& row);
};

std::vector<Sensor> ThingParser::read_sensors(
  const std::string& serial_number
) {
  std::string path = "./storage/" + serial_number + "_sensors.txt";
  if (std::filesystem::exists(path) == false) {
    return {};
  }

  // Read the first row to determine the columns
  std::vector<std::string> columns;
  std::ifstream in_file(path);
  std::string first_row;
  do {
    std::getline(in_file, first_row);
  } while (first_row == "" && in_file.peek() != std::ifstream::traits_type::eof());

  // If we don't find a first row, exit
  if (first_row == "") {
    return {};
  }

  // Parse the sensors
  std::vector<Sensor> sensors;
  std::vector<std::string> columnNames = ThingParser::parse_row(first_row);
  for (std::string row; std::getline(in_file, row);) {
    std::vector<std::string> sensor_values = ThingParser::parse_row(row);
    sensors.push_back(Sensor(columnNames, sensor_values)); // TODO: Move data into sensor rather than copy
  }

  in_file.close();
  return sensors;
}

std::vector<SensorDataVariant> ThingParser::read_sensor_data(
  const std::string &file_name,
  const Sensor& sensor
) {
  // TODO, will be used for channels reading from a file rather than generating random data. 
  return {};
}

std::vector<std::string> ThingParser::find_thing_data_file_names(
  const std::string &serial_number
) {
  std::string path = "./storage/";
  std::vector<std::string> thing_data_file_names;
  for (const auto& entry: std::filesystem::directory_iterator(path)) {
    std::string path = entry.path();
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);
    if (file_name.starts_with(serial_number)) {
      thing_data_file_names.push_back(file_name);
    }
  }
  return thing_data_file_names;
}

std::vector<std::string> ThingParser::parse_row(const std::string& row) {
  int start = 0, end;
  std::string column;
  std::vector<std::string> columns;
  while ((end = row.find(",", start)) != std::string::npos) {
    column = row.substr(start, end - start);
    start = end + 1;
    columns.push_back(column);
  }
  columns.push_back(row.substr(start));
  return columns;
}

#endif