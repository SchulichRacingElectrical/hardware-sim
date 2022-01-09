/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../thing/sensor.h"
#include "stream.h"
#include <optional>
#include <mutex>
#include <ctime>

class AbstractChannel {
  protected:
    Sensor _sensor;
    unsigned int _channel_id;

  public:
    AbstractChannel(Sensor s, unsigned int c) : _sensor(s), _channel_id(c) {}
    virtual ~AbstractChannel() {}
    virtual void open() = 0;
    virtual void close() = 0;

    friend class Stream;
};

template <typename T> requires (std::is_arithmetic<T>::value)
class Channel: public AbstractChannel {
private:
  volatile T _value = 0;
  std::mutex _lock;
  bool _closed = false;

  /**
   * @brief Temporary function used for MVP. In the future, 
   * channel data will be generated and queued through a 
   * strategy (e.g. random or from a file).
   */
  T _generate_random();

public:
  Channel<T>() = delete;
  template <class Sensor> 
  Channel<T>(Sensor& s, unsigned int c) : AbstractChannel(s, c) {}
  Channel<T>(const Channel<T> &) = delete;
  Channel<T>(const Channel<T> &&) = delete;
  Channel<T> &operator=(const Channel &) = delete;

  /**
   * @brief Opens the channel, a stream is able to request and read values. 
   */
  virtual void open() override;

  /**
   * @brief Closes the channel, read() will return an optional null. 
   */
  virtual void close() override;

  /**
   * @brief Send a value into the channel. To be used for the future when 
   * different data generation strategies are available. 
   */
  void send(T value);

  /**
   * @brief Fetches the next value from the data generator, enqueues the value,
   * and sends the most recent data. 
   */
  std::optional<T> read();
};