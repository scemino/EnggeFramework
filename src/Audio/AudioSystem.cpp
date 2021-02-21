#include <ngf/Audio/SoundBuffer.h>
#include <ngf/Audio/AudioSystem.h>
#include "../System/SdlSystem.h"
#include <SDL_mixer.h>
#include <ngf/System/TimeSpan.h>

namespace ngf {
static AudioSystem *g_pSystem = nullptr;

AudioSystem::AudioSystem() {
  g_pSystem = this;
  SDL_CHECK(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096));
  SDL_CHECK(Mix_AllocateChannels(32));
  Mix_ChannelFinished([](int channel) { g_pSystem->onChannelFinished(channel); });
  m_channels.resize(32);
  for (auto i = 0; i < 32; ++i) {
    m_channels[i].init(*this, i);
  }
}

AudioSystem::~AudioSystem() {
  Mix_CloseAudio();
  g_pSystem = nullptr;
}

std::shared_ptr<SoundHandle> AudioSystem::playSound(ngf::SoundBuffer &buffer,
                                                    int loopTimes,
                                                    const TimeSpan &fadeInTime,
                                                    int channel) {
  if (channel >= 0) {
    m_channels[channel].stop();
    m_channels[channel].setSoundBuffer(&buffer);
    m_channels[channel].play(loopTimes, fadeInTime);
    auto handle = std::make_shared<SoundHandle>(m_channels[channel]);
    m_handles.push_back(handle);
    return handle;
  }

  if (fadeInTime == TimeSpan::Zero) {
    channel = SDL_CHECK_EXPR(Mix_PlayChannel(-1, buffer.m_pChunk, loopTimes - 1));
  } else {
    channel =
        SDL_CHECK_EXPR(Mix_FadeInChannel(-1,
                                         buffer.m_pChunk,
                                         loopTimes - 1,
                                         static_cast<int>(fadeInTime.getTotalMilliseconds())));
  }
  if (channel < 0)
    return std::make_shared<SoundHandle>(m_invalidChannel);

  m_channels[channel].setSoundBuffer(&buffer);
  auto handle = std::make_shared<SoundHandle>(m_channels[channel]);
  m_handles.push_back(handle);
  return handle;
}

void AudioSystem::onChannelFinished(int channel) {
  for (auto it = m_handles.begin(); it != m_handles.end(); ++it) {
    if (it->get()->get().getChannel() == channel) {
      it->get()->m_channel = &m_invalidChannel;
      m_handles.erase(it);
      return;
    }
  }
}

size_t AudioSystem::size() const noexcept { return m_channels.size(); }

AudioSystem::reference AudioSystem::at(size_t index) { return m_channels.at(index); }
AudioSystem::const_reference AudioSystem::at(size_t index) const { return m_channels.at(index); }

AudioSystem::const_iterator AudioSystem::begin() const { return m_channels.begin(); }
AudioSystem::const_iterator AudioSystem::end() const { return m_channels.end(); }

AudioSystem::iterator AudioSystem::begin() { return m_channels.begin(); }
AudioSystem::iterator AudioSystem::end() { return m_channels.end(); }

AudioSystem::const_iterator AudioSystem::cbegin() const { return m_channels.cbegin(); }
AudioSystem::const_iterator AudioSystem::cend() const { return m_channels.cend(); }

}
