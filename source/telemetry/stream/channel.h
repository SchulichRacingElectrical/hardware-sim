/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../thing/sensor.h"
#include "stream.h"
#include <functional>
#include <optional>
#include <mutex>
#include <deque>

class AbstractChannel {
  public:
    Sensor& _sensor;

  public:
    AbstractChannel(Sensor& s) : _sensor(s) {}
    virtual ~AbstractChannel() {}

    virtual void open() = 0;
    virtual void close() = 0;
};

template <typename T> requires (std::is_arithmetic<T>::value)
class Channel: public AbstractChannel {
private:
  volatile T _value = 0;
  std::mutex _lock;
  bool _closed = false;

public:
  Channel<T>() {}
  Channel<T>(Sensor& s) : AbstractChannel(s) {}

  virtual void open() override;
  virtual void close() override;

  void send(T value);
  T read() const;
};