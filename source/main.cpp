/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include <iostream>
#include <telemetry/thing/telemetry_thing.h>

int main() {
  // Start the program, say hello world
  // Ask the user how many objects they would like to spawn
  // Give confirmation, then start streaming sensors 
  // On cancellation, the user can either start another session, or quit. 
  // Note that in the future, all of this will be done through UI.

  TelemetryThing *thing = new TelemetryThing("Toaster", "xxx");
  thing->start_telemetry();

  // TelemetryThing *other_thing = new TelemetryThing("SR21", "yyy");
  // other_thing->start_telemetry();

  std::cin.get();
}

// MVP:
// No serious tests, except for the encoder
// Just get to a point where we can spawn some objects with some sensors. 
// Create a nice readme file and make sure the project can be started from scratch. 
// Try getting to a point where everything can be run after a build. 