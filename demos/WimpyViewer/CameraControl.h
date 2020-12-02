#pragma once
#include <ngf/Application.h>
#include <ngf/System/Cursor.h>
#include <ngf/System/Event.h>
#include <glm/vec2.hpp>
#include "Room.h"

class CameraControl {
public:
  explicit CameraControl(Room &room) : m_room(room) {}

  void onEvent(ngf::Application &app, ngf::Event &event) {
    auto target = app.getRenderTarget();
    if (!target)
      return;

    switch (event.type) {
    case ngf::EventType::KeyPressed:m_panEnabled = event.key.scancode == ngf::Scancode::Space;
      m_zoomEnabled = event.key.scancode == ngf::Scancode::LeftShift;
      if (m_panEnabled) {
        m_cursor.setType(ngf::Cursor::Type::Hand);
        app.getWindow().setMouseCursor(m_cursor);
      }
      break;
    case ngf::EventType::KeyReleased:
      if (event.key.scancode == ngf::Scancode::Space) {
        m_panEnabled = false;
        m_cursor.setType(ngf::Cursor::Type::Arrow);
        app.getWindow().setMouseCursor(m_cursor);
      } else if (event.key.scancode == ngf::Scancode::LeftShift) {
        m_zoomEnabled = false;
      }
      break;
    case ngf::EventType::MouseButtonPressed:
      if (m_panEnabled) {
        m_posPressed = target->mapPixelToCoords(event.mouseButton.position);
        m_mouseButtonDown = true;
      }
      break;
    case ngf::EventType::MouseMoved:
      if (m_mouseButtonDown) {
        const auto scale = ngf::Window::getDpiScale();
        const auto pos = target->mapPixelToCoords(event.mouseMoved.position);
        glm::vec2 delta = {m_posPressed.x - pos.x, pos.y - m_posPressed.y};
        m_room.getCamera().position += delta * scale;
        m_posPressed = pos;
      }
      break;
    case ngf::EventType::MouseButtonReleased:m_mouseButtonDown = false;
      break;
    case ngf::EventType::MouseWheelScrolled:
      if (m_zoomEnabled) {
        auto zoom = m_room.getCamera().zoom + event.mouseWheelEvent.offset.y * 0.1f;
        m_room.getCamera().zoom = std::clamp(zoom, 0.1f, 10.f);
      }
      break;
    default:break;
    }
  }

private:
  bool m_panEnabled{false};
  bool m_zoomEnabled{false};
  bool m_mouseButtonDown{false};
  glm::vec2 m_posPressed;
  Room &m_room;
  ngf::Cursor m_cursor;
};