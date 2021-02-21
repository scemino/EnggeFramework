#include <ngf/Audio/SoundBuffer.h>

namespace ngf {
void SoundBuffer::loadFromFile(const std::filesystem::path &path) {
  m_path = path;
  free();
  m_pChunk = Mix_LoadWAV(path.string().c_str());
}

void SoundBuffer::loadFromMemory(const void *data, std::size_t sizeInBytes) {
  free();
  m_pChunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(data, sizeInBytes), 1);
}

SoundBuffer::~SoundBuffer() {
  free();
}

void SoundBuffer::free() {
  if (m_pChunk) {
    Mix_FreeChunk(m_pChunk);
    m_pChunk = nullptr;
  }
}
}