/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "channel.h"
#include <unordered_map>
#include <any>

class Stream {
  private:
    std::unordered_map<unsigned int, Channel<std::any>> channels;

  public:
    Stream();
    ~Stream();

    void create_channels(); // Pass sensors
    std::any read_channel(unsigned int channel_id);
};