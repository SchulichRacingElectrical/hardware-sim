/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#ifndef STREAM_H
#define STREAM_H

#include "channel.h"
#include <telemetry/thing/sensor.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <vector>
#include <future>
#include <chrono>
#include <thread>
#include <limits>
#include <mutex>

/**
 * @brief Data publication callback; used to receive variable frequency
 * channel data from a Stream object. Subscribe and unsubscribe callbacks
 * with Stream::subscribe, Stream::subscribe. Passes a timestamp to specify
 * the time of the data. 
 */
using ReadCallback = std::function<void(unsigned int, std::vector<SensorVariantPair>)>;

/**
 * @brief The gathering point for sensor data. The stream reads from channels and holds 
 * a buffer of the most recent values. The system will handle frequency synchronization, 
 * "significant" data updates (only send data if it has changed significantly). The stream
 * is thread-safe and may be used by multiple threads. 
 */
class Stream {
private:
  /**
   * @brief A map of abstract channels that may be reinterpret casted to a specialization.
   */
  std::unordered_map<unsigned int, AbstractChannel*> _channels;

  /**
   * @brief The most recent values of sensors in the system.
   * The key is the timestamp.
   */
  std::unordered_map<unsigned int, SensorDataVariant> _stream_buffer;

  /**
   * @brief Callbacks that are called when new data is available. Callbacks will be
   * executed at the highest frequency of all channels. 
   */
  std::unordered_map<unsigned int, ReadCallback> _callbacks;

  /**
   * @brief The timestamp of the stream when it is in use in ms. Used internally to
   * determine when sensor values should be read. 
   */
  volatile unsigned int _timestamp = 0;

  /**
   * @brief The maximum frequency of all channels; the frequency the stream runs at. 
   */
  unsigned char _frequency = 0;

  std::thread _read_thread;
  std::mutex _lock;
  bool _closed = true;

  /**
   * @brief An internal method that reads from channels at the highest 
   * channel frequency and publishes data to subscribers. Run via 
   * internal _read_thread. 
   * The function uses a spinlock and is started when Stream::open 
   * is called. The function is exited when Stream::close is called. 
   * The stream is paused and unpaused via Stream::pause, Stream::unpause. 
   */
  void _tap_channels() noexcept;

public:
  Stream() = delete;
  Stream(std::vector<Sensor> &sensors);
  ~Stream();

  /**
   * @brief Subscribe to the stream with a callback using an id. 
   * Returns false if the given id already has a subscriber. 
   * Returns true if the callback is bound successfully. 
   */
  bool subscribe(const unsigned int& id, ReadCallback callback) noexcept;

  /**
   * @brief Unsubscribe the callback associated with the provided ID.
   * Returns false if the provided id does not match with a callback. 
   * Returns true if the provided id's callback was unbound.
   */
  bool unsubscribe(const unsigned int& id) noexcept;

  /**
   * @brief Opens the stream to start reading from channel. 
   * Sets the pause state of the stream to false. 
   */
  void open() noexcept;

  /**
   * @brief Closes the stream and stops reading from channels. 
   * Sets the pause state of the stream to true.
   */
  void close() noexcept;

  /**
   * @brief Returns whether the stream is open or not. 
   */
  bool is_open() const noexcept;
};

#endif