/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#include "telemetry_handler.h"

void TelemetryHandler::run() {
  this->print_instructions();
  bool any_started = false;
  for (const auto &thing : this->_state_container->get_things())
    any_started = any_started || thing->start_telemetry();
  if (any_started) {
    std::cout << GREEN << "Telemetry simulation is starting. Enter 'stop' to gracefully shut down." << RESET << std::endl;
    while (1) {
      std::string line;
      std::getline(std::cin, line);
      if (line == "stop") {
        for (const auto& thing: this->_state_container->get_things()) 
          thing->stop_telemetry();
        std::cout << GREEN << "Telemetry simulation stopped." << RESET << std::endl;
        break;
      }
    }
  } else {
    std::cout << RED << "All Things failed to start telemetry." << RESET << std::endl;
  }
}

void TelemetryHandler::print_instructions() {
  std::cout << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << BLUE << "Your telemetry session will begin shortly." << RESET << std::endl;
}