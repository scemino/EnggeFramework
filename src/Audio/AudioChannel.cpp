#include <algorithm>
#include <SDL_mixer.h>
#include <ngf/Audio/AudioChannel.h>
#include <ngf/Audio/SoundBuffer.h>
#include "../System/SdlSystem.h"

namespace ngf {
void AudioChannel::play(int loopTimes, const TimeSpan &fadeInTime) {
  if (m_channel < 0 || !m_buffer)
    return;
  switch (getStatus()) {
  case Status::Paused:SDL_CHECK(Mix_Resume(m_channel));
    break;
  case Status::Stopped:m_numLoops = loopTimes;
    if (fadeInTime == TimeSpan::Zero) {
      SDL_CHECK(Mix_PlayChannel(m_channel, m_buffer->m_pChunk, loopTimes - 1));
    } else {
      SDL_CHECK(Mix_FadeInChannel(m_channel,
                                  m_buffer->m_pChunk,
                                  loopTimes - 1,
                                  static_cast<int>(fadeInTime.getTotalMilliseconds())));
    }
    break;
  case Status::Playing:return;
  }
  if (m_channel < 0)
    return;
  setVolume(m_volume);
  setPanning(m_panning);
}

void AudioChannel::pause() {
  if (m_channel < 0)
    return;
  SDL_CHECK(Mix_Pause(m_channel));
}

void AudioChannel::stop(const TimeSpan &fadeOutTime) {
  if (m_channel < 0)
    return;
  if (fadeOutTime == TimeSpan::Zero) {
    SDL_CHECK(Mix_HaltChannel(m_channel));
  } else {
    SDL_CHECK(Mix_FadeOutChannel(m_channel, static_cast<int>(fadeOutTime.getTotalMilliseconds())));
  }
}

AudioChannel::Status AudioChannel::getStatus() const {
  if (m_channel < 0)
    return Status::Stopped;
  if (SDL_CHECK_EXPR(Mix_Paused(m_channel)))
    return Status::Paused;
  if (SDL_CHECK_EXPR(Mix_Playing(m_channel)))
    return Status::Playing;
  return Status::Stopped;
}

float AudioChannel::getVolume() const {
  return m_volume;
}

void AudioChannel::setVolume(float volume) {
  m_volume = volume;
  if (m_channel < 0)
    return;
  auto vol = static_cast<int>(volume * 128);
  vol = std::clamp(vol, 0, 128);
  SDL_CHECK(Mix_Volume(m_channel, vol));
}

int AudioChannel::getNumLoops() const {
  return m_numLoops;
}

void AudioChannel::setPanning(float pan) {
  m_panning = std::clamp(pan, -1.f, 1.f);
  if (m_channel < 0)
    return;
  auto p = std::clamp(static_cast<float>(m_panning * 128.f), -127.f, 128.f);
  auto left = static_cast<Uint8>(128.f - p);
  SDL_CHECK(Mix_SetPanning(m_channel, left, 255 - left));
}

[[nodiscard]] float AudioChannel::getPanning() const { return m_panning; }

[[nodiscard]] int AudioChannel::getChannel() const { return m_channel; }

void AudioChannel::init(AudioSystem &system, int channel) {
  m_system = &system;
  m_channel = channel;
}

void AudioChannel::setSoundBuffer(SoundBuffer *buffer) {
  m_buffer = buffer;
}
}