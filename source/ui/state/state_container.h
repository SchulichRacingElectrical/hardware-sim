/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#ifndef STATE_CONTAINER_H
#define STATE_CONTAINER_H

#include <vector>
#include <terminal_consts.h>
#include "../../telemetry/thing/telemetry_thing.h"

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
    bool is_authed() const;
    bool fetch_things();
    const std::vector<std::unique_ptr<TelemetryThing>>& get_things() const;
    void print_things() const;
};

#endif