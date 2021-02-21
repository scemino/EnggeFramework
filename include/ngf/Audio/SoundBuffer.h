#pragma once
#include <filesystem>
#include <SDL_mixer.h>

namespace ngf {
/// @brief Storage for audio samples defining a sound.
class SoundBuffer {
  friend class Sound;
  friend class AudioSystem;
  friend class AudioChannel;

public:
  /// @brief Destructor of a SoundBuffer.
  ~SoundBuffer();
  /// @brief Loads the sound buffer from a file.
  /// \param path Path of the sound file to load.
  void loadFromFile(const std::filesystem::path &path);
  /// @brief Loads the sound buffer from memory.
  /// \param data Pointer to the file data in memory.
  /// \param sizeInBytes Size of the data to load, in bytes.
  void loadFromMemory (const void *data, std::size_t sizeInBytes);

private:
  void free();

private:
  std::filesystem::path m_path;
  Mix_Chunk *m_pChunk{nullptr};
};
}
