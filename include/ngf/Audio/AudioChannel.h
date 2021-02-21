#pragma once
#include <memory>
#include <ngf/System/TimeSpan.h>

namespace ngf {
class AudioSystem;
class SoundBuffer;

class AudioChannel final {
private:
  friend class AudioSystem;

public:
  enum class Status {
    Stopped, //!< Sound is stopped
    Paused,  //!< Sound is paused
    Playing  //!< Sound is playing
  };

public:
  /// @brief Starts or resumes playing the sound source.
  /// \param loopTimes Number of times the sound will be looped, if loopTimes = -1, then it loops the sound forever.
  /// \param fadeInTime Time that the fade-in effect should take to go from silence to full volume.
  void play(int loopTimes = 1, const TimeSpan &fadeInTime = TimeSpan::Zero);

  /// @brief Pauses the sound source.
  void pause();

  /// @brief Stops playing the sound source.
  void stop(const TimeSpan &fadeOutTime = TimeSpan::Zero);

  /// @brief Gets the current status of the sound (stopped, paused, playing)
  /// \return The current status of the sound.
  [[nodiscard]] Status getStatus() const;

  /// @brief Gets the volume of the sound.
  /// \return Volume of the sound, in the range [0.f, 1.f].
  [[nodiscard]] float getVolume() const;

  /// @brief Sets the volume of the sound.
  /// @param volume Volume of the sound to set.
  void setVolume(float volume);

  /// @brief Gets the number of loops the sound will do when it will be played.
  /// \return the number of loops the sound will do when it will be played.
  [[nodiscard]] int getNumLoops() const;

  /// @brief Sets the sound panning.
  /// \param pan Pan position between [-1.f, 1.f]
  void setPanning(float pan);
  /// @brief Gets the sound panning between [-1.f, 1.f]
  /// \return the sound panning.
  [[nodiscard]] float getPanning() const;

  /// @brief Gets the channel number used when sound is played.
  /// \return The channel number used when sound is played.
  [[nodiscard]] int getChannel() const;

  void setSoundBuffer(SoundBuffer *buffer);

private:
  void init(AudioSystem &system, int channel);

private:
  AudioSystem *m_system{nullptr};
  int m_channel{-1};
  int m_numLoops{1};
  float m_volume{1.f};
  float m_panning{0.f};
  SoundBuffer *m_buffer{nullptr};
};
}