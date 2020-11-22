#pragma once
#include <functional>
#include <glm/vec2.hpp>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/System/Event.h>
#include <ngf/Math/Math.h>

namespace ngf{
enum class ButtonEventType {
  Enter,
  Leave,
  Down,
  Up,
  Drag,
  Drop
};

using Callback = std::function<void(ButtonEventType, glm::vec2)>;

template<typename T>
struct InputArea {
  const T *pImage{nullptr};
  Callback callback;
  bool isOver{false};
  bool isDragging{false};
  glm::vec2 startDragPos{};
};

template<typename T>
void manageAreaEvent(const ngf::RenderTarget &target, InputArea<T> &area, const ngf::Event &event) {
  switch (event.type) {
  case ngf::EventType::MouseMoved: {
    auto pos = target.mapPixelToCoords(event.mouseMoved.position);
    auto isOver = ngf::contains(*area.pImage, pos);
    if (area.isOver != isOver) {
      area.isOver = isOver;
      area.callback(isOver ? ButtonEventType::Enter : ButtonEventType::Leave, pos);
    } else if (area.isDragging) {
      area.callback(ButtonEventType::Drag, pos);
    }
  }
    break;
  case ngf::EventType::MouseButtonPressed: {
    auto pos = target.mapPixelToCoords(event.mouseMoved.position);
    if (area.isOver) {
      area.startDragPos = pos;
      area.isDragging = true;
      area.callback(ButtonEventType::Down, pos);
    }
  }
    break;
  case ngf::EventType::MouseButtonReleased: {
    auto pos = target.mapPixelToCoords(event.mouseMoved.position);
    if (area.isOver) {
      area.callback(ButtonEventType::Up, pos);
    }
    if (area.isDragging) {
      area.isDragging = false;
      area.callback(ButtonEventType::Drop, pos);
    }
  }
    break;
  default:break;
  }
}

template<typename T>
InputArea<T> createArea(const T &object, const Callback &callback) {
  return InputArea<T>{&object, callback};
}
};