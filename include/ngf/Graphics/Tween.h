#pragma once
#include <functional>
#include <cstdint>
#include <ngf/Math/Math.h>
#include <ngf/System/TimeSpan.h>
#include <ngf/System/Times.h>
#include <ngf/Math/Easing.h>

namespace ngf {
using EasingFunction = float (*)(float);

/// @brief Behavior specifying how an interpolation will be repeated.
///
/// If the repeat behavior is OneWay, when the interpolation is done
/// it will start again from the from value to the to value.
/// If the repeat behavior is TwoWay, when the interpolation is done
/// it will interpolate back to the from value.
enum class RepeatBehavior {
  OneWay, ///< One way behavior, when the interpolation is done, it will start again from the from value
  TwoWay ///< Two way behavior, when the interpolation is done, it will interpolate back to the from value
};

/// @brief Interpolation between any type of value.
/// \tparam T type of the value to interpolate
template<typename T>
class Tween {
public:
  /// @brief The type of the callback to be called when the value is changed.
  using ValueChangedCallback = std::function<void(const T &)>;

public:
  /// @brief Create an empty interpolation.
  Tween() noexcept = default;

  /// @brief Create an interpolation between from and to.
  /// \param from The value to begin with.
  /// \param to The value to end with.
  Tween(T from, T to) noexcept
      : m_from(from), m_to(to) {
  }

  /// @brief Sets the callback to call at each update.
  /// \param onValueChanged This callback will be called at each update.
  /// \return the interpolation with this new callback.
  Tween& onValueChanged(ValueChangedCallback onValueChanged) noexcept {
    m_onValueChanged = onValueChanged;
    return *this;
  }

  /// @brief Sets the duration of the interpolation.
  /// \param duration The duration of the interpolation.
  /// \return the interpolation with this new duration.
  Tween& setDuration(const TimeSpan &duration) noexcept {
    m_done = duration == TimeSpan::seconds(0);
    m_duration = duration;
    return *this;
  }

  /// @brief Sets the easing function.
  /// \param easing The easing function to use during the interpolation.
  /// \return the interpolation with this new easing function.
  Tween& with(EasingFunction easing) noexcept {
    m_easing = easing;
    return *this;
  }

  /// @brief Sets the number of times the interpolation has to be repeated and how it is repeated.
  /// \param times The number of times the interpolation has to be repeated.
  /// \param behavior The behavior to use when the interpolation is repeated.
  /// \return the interpolation which will be repated with these new parameters.
  Tween& repeat(Times times, RepeatBehavior behavior = RepeatBehavior::OneWay) noexcept {
    m_times = times;
    m_repeatBehavior = behavior;
    return *this;
  }

  /// @brief Gets the value of the current interpolation.
  /// \return The value of the current interpolation.
  T getValue() const noexcept {
    auto t = m_elapsed.getTotalSeconds() / m_duration.getTotalSeconds();
    if (!m_forward)
      t = 1.f - t;
    return lerp(m_from, m_to, m_easing(t));
  }

  /// @brief Returns a value indicating whether or not the interpolation is done.
  /// \return true if the interpolation is done or false if still in progress.
  [[nodiscard]] constexpr bool isDone() const noexcept {
    return m_done;
  }

  /// @brief Updates the interpolation with the specified elapsed time.
  /// \param elapsed This the time elapsed since last time it has been called.
  void update(const TimeSpan &elapsed) noexcept {
    if (m_done)
      return;

    m_elapsed += elapsed;

    if (m_elapsed >= m_duration) {
      if (m_times < 1) {
        m_elapsed = m_duration;
        if (m_onValueChanged)
          m_onValueChanged(getValue());
        m_done = true;
        m_times = 0;
        return;
      }
      m_elapsed -= m_duration;
      if (m_repeatBehavior == RepeatBehavior::TwoWay) {
        m_forward = !m_forward;
      }
      if (m_times != Times::Infinity) {
        --m_times;
      }
    }

    if (m_onValueChanged)
      m_onValueChanged(getValue());
  }

private:
  T m_from;
  T m_to;
  ValueChangedCallback m_onValueChanged{nullptr};
  TimeSpan m_elapsed{};
  TimeSpan m_duration{};
  EasingFunction m_easing{Easing::linear};
  bool m_done{true};
  Times m_times;
  bool m_forward{true};
  RepeatBehavior m_repeatBehavior{RepeatBehavior::OneWay};
};

class Tweening {
public:
  /// @brief Create an interpolation from the 'from' value to the 'to' value.
  /// \tparam T Type of the value to interpolate.
  /// \param from The interpolation will begin from this value.
  /// \param to The interpolation will end when the value 'to' will be reached.
  /// \return The interpolation created.
  template<typename T>
  static Tween<T> make(T from, T to) noexcept { return ngf::Tween(from, to); }
};
}
