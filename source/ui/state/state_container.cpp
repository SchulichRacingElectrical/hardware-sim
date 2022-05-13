/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "state_container.h"

bool StateContainer::request_auth(std::string key) {
  std::string endpoint = "/api/database/auth/validate";
  httplib::Client client(this->_web_address);
  client.set_read_timeout(100000);
  httplib::Headers headers = {{"apiKey", key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      this->_api_key = key;
      this->_authed = true;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool StateContainer::is_authed() const {
  return this->_authed;
}

bool StateContainer::fetch_things() {
  std::string endpoint = "/api/database/things";
  httplib::Client client(this->_web_address);
  client.set_read_timeout(100000);
  httplib::Headers headers = {{"apiKey", _api_key}};
  if (auto res = client.Get(std::move(endpoint.c_str()), headers)) {
    if (res->status == 200) {
      json body = json::parse(res->body).at("data");
      for (json::iterator it = body.begin(); it != body.end(); ++it) {
        json element = *it; // TODO: What if this fails?
        std::string name = element.at("name");
        std::string serial_number = element.at("_id");
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

const std::vector<std::unique_ptr<TelemetryThing>>& StateContainer::get_things() const {
  return _things;
}

void StateContainer::print_things() const {
  if (this->_things.size() == 0) {
    std::cout << std::endl;
    std::cout << RED << "No Things are available." << RESET << std::endl;
  } else {
    std::cout << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << BLUE << "Serial Number           , Name" << RESET << std::endl;
    for (const auto& thing: this->_things) 
      std::cout << *thing << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
  }
}
