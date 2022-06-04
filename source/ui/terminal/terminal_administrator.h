/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef TERMINAL_ADMINISTRATOR_H
#define TERMINAL_ADMINISTRATOR_H

#include <iostream>
#include <ostream>
#include <terminal_consts.h>
#include "telemetry_handler.h"
#include "../state/state_container.h"
#include <filesystem>
#include <stdlib.h>

class TerminalAdministrator {
  private:
    void print_introduction();
    void print_intructions();
    void print_loading(std::string message);
    void print_error(std::string message);
    void print_message(std::string message);
    void print_auth_request();
    void print_things();
    void handle_auth();

    bool _quit = false;
    StateContainer _state_container;

  public:
    TerminalAdministrator() { _state_container = StateContainer(); }
    void run();
};

#endif