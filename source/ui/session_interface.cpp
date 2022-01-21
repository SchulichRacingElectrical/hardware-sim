/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "session_interface.h"

void SessionInterface::begin_session() {
  std::filesystem::create_directory("storage");
  _handle_input();
}

void SessionInterface::_handle_input() {
  while (1) {
    if (_api_key == "") {
      std::cout << "What is your API Key?" << std::endl;
      std::getline(std::cin, _api_key);
      _fetch_things();
      if (_api_key == "") {
        std::cout << "Looks like your API Key isn't valid, please try again." << std::endl;
      }
    } else {
      if (_things.size() == 0) {
        std::cout << "Looks like you don't have any things in your database, please add some and come back." << std::endl;
        break;
      }
      _print_instructions();
      std::string line;
      std::getline(std::cin, line);
      if (line == "view") {
        _print_things();
        _print_session_things();
        // Print out _things
        // Print out _session_things
        // Allow the user to remove from _session_things
      } else if (line == "thing") {
        _print_session_things();
        std::cout << "Enter the serial number of the thing you want to add to the session." << std::endl;
        std::getline(std::cin, line);
        // Try to find the thing in _things, make sure it does not exist in _session_things
        // If does not exist, notify
        // If does exist, add to _session_things, display session things
      } else if (line == "start") {
        if (_session_things.size() == 0) {
          std::cout << "You don't have any things in your session yet!" << std::endl;
        } else {
          std::cout << "Starting the telemetry session! Note that the console will get overflown." << std::endl;
          std::cout << "Note that multiple thing logs will collide as they run async." << std::endl;
          std::cout << "Enter 'stop' at any time to stop the stream." << std::endl;
          sleep(3);
          for (auto& thing: _session_things) {
            thing.start_telemetry();
          }
          while (1) {
            std::getline(std::cin, line);
            if (line == "stop") {
              for (auto& thing: _session_things) {
                thing.stop_telemetry();
              }
              break;
            }
          }
        }
      } else if (line == "q") {
        std::cout << "Thanks for stopping by!" << std::endl;
        break;
      } else {
        std::cout << "Invalid input, please try again." << std::endl;
      }
    }
  }
}

void SessionInterface::_print_instructions() {
  std::cout << "Enter 'view' to see which 'things' currently exist." << std::endl;
  std::cout << "Enter 'thing' to a create add a thing that will stream telemetry." << std::endl;
  std::cout << "Enter 'start' to begin telemetry for your 'things'." << std::endl;
  std::cout << "Enter 'q' to quit the program at any time." << std::endl;
}

void SessionInterface::_fetch_things() {

}