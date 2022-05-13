/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#include "telemetry_handler.h"

void TelemetryHandler::run() {
  this->print_instructions();
  for (const auto& thing: this->_state_container->get_things()) {
    thing->start_telemetry();
  }
}

void TelemetryHandler::print_instructions() {
  std::cout << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << BLUE << "Your telemetry session will begin shortly." << RESET << std::endl;
}

void TelemetryHandler::begin_simulation() {

}