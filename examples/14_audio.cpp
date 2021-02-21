#include <ngf/Application.h>
#include <imgui.h>
#include <ngf/Audio/SoundBuffer.h>

namespace {
std::string getStatus(ngf::AudioChannel::Status status) {
  std::string statusText;
  switch (status) {
  case ngf::AudioChannel::Status::Paused: statusText = "Paused";
    break;
  case ngf::AudioChannel::Status::Stopped: statusText = "Stopped";
    break;
  case ngf::AudioChannel::Status::Playing:statusText = "Playing";
    break;
  }
  return statusText;
}
}

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({"14_Audio", {640, 480}});

    m_soundBuffer.loadFromFile("./assets/Sad Frog Song_MNm.ogg");

    for (auto &sound : m_audioSystem) {
      sound.setSoundBuffer(&m_soundBuffer);
    }
    m_soundId = m_audioSystem.playSound(m_soundBuffer);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::LightBlue);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    auto id = m_soundId->get().getChannel();
    ImGui::Text("Id: %d", id);
    ImGui::InputInt("Num Loops", &m_loops);
    ImGui::InputFloat("Fade in (ms)", &m_fadeIn);
    ImGui::InputFloat("Fade out (ms)", &m_fadeOut);
    ImGui::Separator();
    if (ImGui::BeginTable("Sounds", 6)) {
      ImGui::TableSetupColumn("id");
      ImGui::TableSetupColumn("status");
      ImGui::TableSetupColumn("loops");
      ImGui::TableSetupColumn("control");
      ImGui::TableSetupColumn("volume");
      ImGui::TableSetupColumn("panning");
      ImGui::TableHeadersRow();
      for (auto &sound : m_audioSystem) {
        ImGui::PushID(&sound);
        ImGui::TableNextRow();
        // id
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("#%d", sound.getChannel());
        // status
        ImGui::TableSetColumnIndex(1);
        auto status = sound.getStatus();
        auto statusText = getStatus(status);
        ImGui::Text("%s", statusText.c_str());
        ImGui::TableNextColumn();
        // loops
        ImGui::TableSetColumnIndex(2);
        auto loops = sound.getNumLoops();
        ImGui::Text("%d", loops);
        // control
        ImGui::TableSetColumnIndex(3);
        switch (status) {
        case ngf::AudioChannel::Status::Stopped: {
          if (ImGui::Button("Play")) {
            sound.play(m_loops, ngf::TimeSpan::milliseconds(m_fadeIn));
          }
          break;
        }
        case ngf::AudioChannel::Status::Paused: {
          if (ImGui::Button("Play")) {
            sound.play(m_loops, ngf::TimeSpan::milliseconds(m_fadeIn));
          }
          ImGui::SameLine();
          if (ImGui::Button("Stop")) {
            sound.stop(ngf::TimeSpan::milliseconds(m_fadeOut));
          }
          break;
        }
        case ngf::AudioChannel::Status::Playing: {
          if (ImGui::Button("Stop")) {
            sound.stop(ngf::TimeSpan::milliseconds(m_fadeOut));
          }
          ImGui::SameLine();
          if (ImGui::Button("Pause")) {
            sound.pause();
          }
        }
        }
        ImGui::TableNextColumn();
        // volume
        ImGui::TableSetColumnIndex(4);
        auto volume = sound.getVolume();
        if (ImGui::SliderFloat("", &volume, 0, 1.f)) {
          sound.setVolume(volume);
        }
        // panning
        ImGui::TableSetColumnIndex(5);
        auto panning = sound.getPanning();
        if (ImGui::SliderFloat("", &panning, -1.f, 1.f)) {
          sound.setPanning(panning);
        }
        ImGui::PopID();
      }
      ImGui::EndTable();
    }
    ImGui::End();
  }

private:
  ngf::SoundBuffer m_soundBuffer;
  int m_loops{1};
  float m_fadeIn{0.f};
  float m_fadeOut{0.f};
  std::shared_ptr<ngf::SoundHandle> m_soundId;
};

int main(int argc, char *argv[]) {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}