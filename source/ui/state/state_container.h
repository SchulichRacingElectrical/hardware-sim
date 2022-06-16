/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/

#ifndef STATE_CONTAINER_H
#define STATE_CONTAINER_H

#include <vector>
#include <terminal_consts.h>
#include "../../telemetry/thing/telemetry_thing.h"

// 6ae7867a-b5d3-4b5d-bc68-1b4a53c14296

class StateContainer {
  private:
    std::string _web_address = "http://199.116.235.51:8080";
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