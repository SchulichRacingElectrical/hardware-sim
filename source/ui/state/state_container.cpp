/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "state_container.h"

bool StateContainer::request_auth(std::string key) {
  std::string endpoint = "/database/auth/validate";
  httplib::Client client(this->_web_address);
  httplib::Headers headers = {{"apiKey", _api_key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      this->_api_key = key;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool StateContainer::is_authed() {
  return this->_authed;
}

bool StateContainer::fetch_things() {
  std::string endpoint = "/database/auth/validate";
  httplib::Client client(this->_web_address);
  httplib::Headers headers = {{"apiKey", _api_key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      json body = json::parse(res->body);
      for (json::iterator it = body.begin(); it != body.end(); ++it) {
        std::string name = (*it)["name"];
        std::string serial_number = (*it)["_id"];
        this->_things.push_back(std::make_unique<TelemetryThing>(name, serial_number, this->_api_key));
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void StateContainer::print_things() {
  for (const auto& thing: this->_things) {
    // Print it out
  }
}
