/*
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

class TelemetryHandler {
  private:
    void print_instructions();
    void show_options();
    void handle_thing_selection();
    void begin_simulation();

  public:
    TelemetryHandler() {}
    void run();
};