/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "stream.h"

Stream::Stream(std::unordered_map<unsigned char, Sensor> &sensors) {
  for (auto it = sensors.begin(); it != sensors.end(); it++) {
    std::visit(
      [&](auto v) {
        // Create a new channel
        AbstractChannel* channel = new Channel<decltype(v)>(it->second);
        _channels[it->second._channel_id] = channel;
        if (_frequency < it->second._frequency)
          _frequency = it->second._frequency;

        // Create the stream buffer entry for the channel
        decltype(v) variant_type = 0;
        _stream_buffer[it->second._channel_id] = variant_type;
      },
      it->second.get_variant()
    );
  }
}

Stream::~Stream() {
  for (auto it = _channels.begin(); it != _channels.end(); it++) {
    delete it->second;
  }
}

void Stream::_tap_channels() noexcept {
  auto spin_lock_time = std::chrono::milliseconds((1 / _frequency) * 1000);
  while (1) {
    // Narrow the lock scope
    {
      std::lock_guard<std::mutex> safe_lock(_lock);
      if (_closed)
        return;
      if (_paused)
        continue;

      // Read from every channel, update if the value has changed significantly
      std::vector<unsigned int> changed;
      for (auto it = _channels.begin(); it != _channels.end(); it++) {
        AbstractChannel *abstract_channel = it->second;
        std::visit(
          [&](auto v) {
            Channel<decltype(v)> *channel = dynamic_cast<Channel<decltype(v)>*>(abstract_channel);
            SensorRange bounds = channel->_sensor._bounds;
            int epsilon = (bounds.upper - bounds.lower) * 0.005f; // Less than 0.5% difference is not significant
            std::visit(
              [&](auto last_value) {
                auto current_value = channel->read();
                if (abs(current_value - last_value) > epsilon) {
                  _stream_buffer[channel->_sensor._channel_id] = current_value;
                  changed.push_back(it->first);
                } 
              }, 
              _stream_buffer[channel->_sensor._channel_id]
            );
          }, 
          abstract_channel->_sensor.get_variant()
        );
      }

      // Create the data that only contains values for this timestep and have significantly changed
      std::vector<SensorVariantPair> data;
      for (const unsigned int& channel_id: changed) {
        unsigned char sensor_id = _channels[channel_id]->_sensor._id;
        auto value = _stream_buffer[channel_id];
        data.emplace_back(sensor_id, _stream_buffer);
      }

      // Callback for subscribers of the stream. 
      for (auto it = _callbacks.begin(); it != _callbacks.end(); it++) {
        it->second(data);
      }
    }
    // Run the loop at the highest channel frequency
    std::this_thread::sleep_for(spin_lock_time);
  }
}

void Stream::open() noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_closed) {
    _paused = false;
    _closed = false;
    _read_thread = std::thread(&Stream::_tap_channels, this);
  }
  if (_paused) _paused = false;
}

void Stream::close() noexcept {
  {
    std::lock_guard<std::mutex> safe_lock(_lock);
    _paused = true;
    _closed = true;
  }
  _read_thread.join();
}

void Stream::pause() noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _paused = true;
}

void Stream::unpause() noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _paused = false;
}

bool Stream::subscribe(unsigned int id, ReadCallback callback) noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_callbacks.find(id) == _callbacks.end()) {
    return false;
  } else {
    _callbacks[id] = callback;
    return true;
  }
}

bool Stream::unsubscribe(unsigned int id) noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_callbacks.find(id) == _callbacks.end()) {
    return false;
  } else {
    _callbacks.erase(id);
    return true;
  }
}