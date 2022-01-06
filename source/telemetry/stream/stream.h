/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include <unordered_map>
#include <any>
#include "channel.h"

class Stream {
  private:
    std::unordered_map<unsigned int, Channel<std::any>> channels;

  public:
    Stream();
    ~Stream();
};