/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef SESSION_INTERFACE_H
#define SESSION_INTERFACE_H

#include <telemetry/thing/telemetry_thing.h>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>

class SessionInterface {
  private:
    std::vector<TelemetryThing> _things;
    std::vector<TelemetryThing&> _session_things;
    std::string _api_key;

    void _print_instructions();
    void _print_session_things();
    void _print_things();
    void _handle_input();
    void _fetch_things();

  public:
    void begin_session();
};

#endif