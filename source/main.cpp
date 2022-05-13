/*
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#include <iostream>
#include <ostream>
#include <ui/terminal/terminal_administrator.h>

#define TERMINAL 1

int main() {
  #if TERMINAL == 1
    TerminalAdministrator administrator = TerminalAdministrator();
    administrator.run();
  #else
    // TODO: Implement Qt GUI
  #endif
  return 0;
}