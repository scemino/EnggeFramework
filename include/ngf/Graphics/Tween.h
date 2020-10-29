#pragma once
#include <functional>
#include <cstdint>
#include <ngf/Math/Math.h>
#include <ngf/System/TimeSpan.h>
#include <ngf/System/Times.h>
#include <ngf/Math/Easing.h>

namespace ngf {
using EasingFunction = float (*)(float);

enum class RepeatBehavior {
  OneWay,
  TwoWay
};

template<typename T>
class Tween {
public:
  using ValueChangedCallback = std::function<void(const T &)>;

private:
  Tween(T from,
        T to,
        ValueChangedCallback onValueChanged,
        const TimeSpan &duration,
        EasingFunction easing,
        Times times, RepeatBehavior repeatBehavior)
      : m_from(from), m_to(to), m_onValueChanged(onValueChanged), m_duration(duration), m_easing(easing),
        m_times(times), m_repeatBehavior(repeatBehavior) {
  }

public:
  Tween() = default;

  Tween(T origin, T target)
      : m_from(origin), m_to(target) {
  }

  Tween onValueChanged(ValueChangedCallback onValueChanged) {
    return Tween(m_from, m_to, onValueChanged, m_duration, m_easing, m_times, m_repeatBehavior);
  }

  Tween setDuration(const TimeSpan &duration) {
    return Tween(m_from, m_to, m_onValueChanged, duration, m_easing, m_times, m_repeatBehavior);
  }

  Tween with(EasingFunction easing) {
    return Tween(m_from, m_to, m_onValueChanged, m_duration, easing, m_times, m_repeatBehavior);
  }

  Tween repeat(Times times, RepeatBehavior behavior = RepeatBehavior::OneWay) {
    return Tween(m_from, m_to, m_onValueChanged, m_duration, m_easing, times, behavior);
  }

  T getValue() const {
    auto t = m_elapsed.getTotalSeconds() / m_duration.getTotalSeconds();
    if (!m_forward)
      t = 1.f - t;
    return lerp(m_from, m_to, m_easing(t));
  }

  void update(const TimeSpan &elapsed) {
    if (m_done)
      return;

    m_elapsed += elapsed;

    if (m_elapsed >= m_duration) {
      m_elapsed = m_duration;
      m_elapsed = ngf::TimeSpan::seconds(0);
      if (m_times <= 1) {
        m_done = true;
        m_times = 0;
        return;
      }
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
  bool m_done{false};
  Times m_times;
  bool m_forward{true};
  RepeatBehavior m_repeatBehavior{RepeatBehavior::OneWay};
};

class Tweening {
public:
  template<typename T>
  static Tween<T> make(T from, T to) { return ngf::Tween(from, to); }
};
}
