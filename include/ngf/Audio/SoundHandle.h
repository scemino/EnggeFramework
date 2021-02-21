#pragma once

namespace ngf {
class AudioChannel;

struct SoundHandle {
public:
  friend class AudioSystem;

public:
  explicit SoundHandle(AudioChannel &channel);
  AudioChannel &get();

private:
  AudioChannel *m_channel{nullptr};
};
}