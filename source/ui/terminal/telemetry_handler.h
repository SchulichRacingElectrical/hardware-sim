/*
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#ifndef TELEMETRY_HANDLER_H
#define TELEMETRY_HANDLER_H

#include <chrono>
#include <iostream>
#include <terminal_consts.h>
#include "../state/state_container.h"

class TelemetryHandler {
  private:
    void print_instructions();
    void begin_simulation();

    // FUTURE: Pick which things we want to send telemetry for
    // void show_options();
    // void handle_thing_selection();

    StateContainer *_state_container;

  public:
    TelemetryHandler(StateContainer* s): _state_container(s) {}
    void run();
};

#endif