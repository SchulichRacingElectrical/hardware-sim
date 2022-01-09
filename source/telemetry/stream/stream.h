/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include "../thing/sensor.h"
#include "channel.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>
#include <thread>

using ReadCallback = std::function<void(std::vector<SensorVariantPair>)>;

class Stream {
private:
  std::unordered_map<unsigned int, AbstractChannel*> _channels;
  std::unordered_map<unsigned int, SensorDataVariant> _stream_buffer;
  std::unordered_map<unsigned int, ReadCallback> _callbacks;
  unsigned int _frequency = 0;

  std::thread _read_thread;
  std::mutex _lock;
  bool _paused = true;
  bool _closed = true;

  /**
   * @brief 
   * 
   */
  void _tap_channels() noexcept;

public: 
  Stream(std::unordered_map<unsigned char, Sensor> &sensors);
  ~Stream();

  /**
   * @brief Subscribe to the stream with a callback; thread safe. 
   * 
   * @param id 
   * @param callback 
   * @return true 
   * @return false 
   */
  bool subscribe(unsigned int id, ReadCallback callback) noexcept;

  /**
   * @brief 
   * 
   * @param id 
   * @return true 
   * @return false 
   */
  bool unsubscribe(unsigned int id) noexcept;

  /**
   * @brief 
   * 
   */
  void open() noexcept;

  /**
   * @brief 
   * 
   */
  void pause() noexcept;

  /**
   * @brief 
   * 
   */
  void close() noexcept;
};