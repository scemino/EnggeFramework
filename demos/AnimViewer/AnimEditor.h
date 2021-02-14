#pragma once
#include <memory>
#include <filesystem>
#include <string>
#include <ngf/Graphics/Sprite.h>
#include <ngf/IO/GGPackValue.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include "Camera.h"
#include "Object.h"
#include "SpriteSheetItem.h"

namespace ngf {
class Application;
}

class AnimEditor final {
public:
  explicit AnimEditor(ngf::Application &application);

  void loadAnim(const std::filesystem::path &path);
  void onRender(ngf::RenderTarget &target);
  void update(const ngf::TimeSpan &e);
  void draw();

private:
  void save();
  void showMainMenuBar();
  void showTools();
  static void update(const ngf::TimeSpan &e, ObjectAnimation &animation);
  void drawAnim(ngf::RenderTarget &target, ngf::RenderStates states);
  void draw(const ObjectAnimation &anim, ngf::RenderTarget &target, ngf::RenderStates states);
  void showAnimationList();
  void showAnimationFrames();
  void showAnimation();
  void newAnimation();
  void newLayer(const std::string &name);
  void showFrames(std::vector<SpriteSheetItem> &items);
  void setSelectAnim(ObjectAnimation *pAnim);
  static void HelpMarker(const char *desc);
  void loadSheet(const std::filesystem::path &path);
  static SpriteSheetItem toSpriteSheetItem(const std::string &name, const ngf::GGPackValue &value);
  static bool toBool(const ngf::GGPackValue &gValue);
  static ngf::irect toRect(const ngf::GGPackValue &value);
  static glm::ivec2 toSize(const ngf::GGPackValue &value);
  static glm::ivec2 parseIVec2(std::string_view value);
  std::vector<ObjectAnimation> parseObjectAnimations(const ngf::GGPackValue &gAnimations);
  ObjectAnimation parseObjectAnimation(const ngf::GGPackValue &gAnimation);

private:
  enum class State {
    Play,
    Pause
  };

  ngf::Application &m_application;
  std::filesystem::path m_path;
  std::string m_sheet;
  ngf::Color m_clearColor{ngf::Colors::LightBlue};
  std::map<std::string, SpriteSheetItem> m_frames;
  std::vector<ObjectAnimation> m_animations;
  ObjectAnimation *m_selectedAnim{nullptr};
  ImGuiTextFilter m_filterAnim;
  ImGuiTextFilter m_filterFrame;
  ngf::TimeSpan m_elapsed;
  std::shared_ptr<ngf::Texture> m_texture;
  AnimControl m_animControl;
  bool m_left{false};
  bool m_showAnimations{false};
  bool m_showTools{false};
  Camera m_camera;
  float m_scale{1.f};
  bool m_loop{false};
  std::string m_newLayerName;
};