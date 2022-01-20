/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef CHANNEL_H
#define CHANNEL_H

#include <telemetry/thing/sensor.h>
#include <optional>
#include <mutex>
#include <ctime>

/**
 * @brief Abstract class that specialized channels inherit from. Used for simplified
 * storage of Channel specializations that can be reinterpreted. 
 */
class AbstractChannel {
  public:
    Sensor sensor;
    AbstractChannel(Sensor s): sensor(s) {}
    virtual ~AbstractChannel() {}
    virtual void open() = 0;
    virtual void close() = 0;
};

/**
 * @brief A templated channel that holds the type of the sensor. Generates data
 * for a Stream. 
 */
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
  Channel<T>(Sensor& s) : AbstractChannel(s) {}
  Channel<T>(const Channel<T>&) = delete;
  Channel<T>(const Channel<T>&&) = delete;
  Channel<T> &operator=(const Channel&) = delete;

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
   * and sends the most recent data. The timestamp is passed as a parameter to
   * read from the correct timestamp of a file. Time stamps are stored at the 
   * highest frequency of all sensors, (i.e. 10Hz means timestamps are multiples of 100).
   * Timestamp may need to passed by copy in the future if this function is called async.
   */
  std::optional<T> read(const volatile unsigned int& timestamp);
};

// Must define inside the header, otherwise Stream cannot resolve specializations
// TODO: Update CMake to resolve linking errors

template <typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::send(T v) {
  if (!_closed) {
    std::lock_guard<std::mutex> safe_lock(_lock);
    _value = v;
  }
}

template<typename T> requires (std::is_arithmetic<T>::value)
std::optional<T> Channel<T>::read(const volatile unsigned int& timestamp) {
  if (_closed) return std::nullopt;
  std::lock_guard<std::mutex> safe_lock(_lock);
  _value = _generate_random();
  return _value;
}

template<typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::open() {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _closed = false;
}

template<typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::close() {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _closed = true;
}

template<typename T> requires (std::is_arithmetic<T>::value)
T Channel<T>::_generate_random() {
  srand(time(nullptr));
  int decision = rand() % 2 + 1;
  switch (decision) {
    case 1:
      return _value + 1;
    case 2:
      return _value - 1;
    default:
      return _value;
  }
}

#endif