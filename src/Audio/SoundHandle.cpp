#include <assert.h>
#include <ngf/Audio/SoundHandle.h>

namespace ngf {
SoundHandle::SoundHandle(AudioChannel &channel) { m_channel = &channel; }

AudioChannel &SoundHandle::get() {
  assert(m_channel);
  return *m_channel;
}
}