#pragma once
#include <memory>
#include <vector>
#include <ngf/Audio/AudioChannel.h>
#include <ngf/Audio/SoundHandle.h>

namespace ngf {
class SoundBuffer;
class Sound;

class AudioSystem final {
public:
  using value_type = std::vector<AudioChannel>::value_type;
  using pointer = std::vector<AudioChannel>::pointer;
  using const_pointer = std::vector<AudioChannel>::const_pointer;
  using reference = std::vector<AudioChannel>::reference;
  using const_reference = std::vector<AudioChannel>::const_reference;
  using iterator_category = std::forward_iterator_tag;
  using iterator = std::vector<AudioChannel>::iterator;
  using const_iterator = std::vector<AudioChannel>::const_iterator;

public:
  AudioSystem();
  ~AudioSystem();

  std::shared_ptr<SoundHandle> playSound(ngf::SoundBuffer &buffer,
                                         int loopTimes = 1,
                                         const TimeSpan &fadeInTime = TimeSpan::Zero,
                                         int channel = -1);

  [[nodiscard]] size_t size() const noexcept;

  reference at(size_t index);
  [[nodiscard]] const_reference at(size_t index) const;

  [[nodiscard]] const_iterator begin() const;
  [[nodiscard]] const_iterator end() const;

  [[nodiscard]] iterator begin();
  [[nodiscard]] iterator end();

  [[nodiscard]] const_iterator cbegin() const;
  [[nodiscard]] const_iterator cend() const;

private:
  void onChannelFinished(int channel);
  std::vector<AudioChannel> m_channels;
  AudioChannel m_invalidChannel;
  std::vector<std::shared_ptr<SoundHandle>> m_handles;
};
}
