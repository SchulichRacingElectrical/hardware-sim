/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#include <vector>
#include "../../telemetry/thing/telemetry_thing.h"
#include "../terminal/terminal_consts.h"

// 61c22001-b57e-4294-bceb-b8908cff4e4c

class StateContainer {
  private:
    std::string _web_address = "http://localhost:5001";
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