/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "channel.h"
#include <unordered_map>

class ChannelFactory {
  private:
    std::unordered_map<unsigned long int, AbstractChannel> _channels;

  public:
};