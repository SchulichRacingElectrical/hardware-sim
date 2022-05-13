/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#include <vector>
#include "../../telemetry/thing/telemetry_thing.h"

class StateContainer {
  private:
    std::string _web_address;
    std::string _api_key;
    std::vector<std::unique_ptr<TelemetryThing>> _things;
    bool _authed;

  public:
    StateContainer() {}
    bool request_auth(std::string key);
    bool is_authed();
    bool fetch_things();
    void print_things();
};