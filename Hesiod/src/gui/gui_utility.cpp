#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

// utility

void flip_vertically(int width, int height, uint8_t *data)
{
  char rgb[3];
  for (int y = 0; y < height / 2; ++y)
    for (int x = 0; x < width; ++x)
    {
      int top = (x + y * width) * 3;
      int bottom = (x + (height - y - 1) * width) * 3;

      memcpy(rgb, data + top, sizeof(rgb));
      memcpy(data + top, data + bottom, sizeof(rgb));
      memcpy(data + bottom, rgb, sizeof(rgb));
    }
}

void save_screenshot(std::string fname)
{
  // https://github.com/vallentin/GLCollection/blob/master/screenshot.cpp
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  int x = viewport[0];
  int y = viewport[1];
  int width = viewport[2];
  int height = viewport[3];

  std::vector<uint8_t> img(width * height * 3);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img.data());

  flip_vertically(width, height, img.data());
  hmap::write_png_rgb_8bit(fname, img, hmap::Vec2<int>(width, height));
}

void main_dock(hesiod::vnode::ViewTree &view_tree)
{
  ImGuiIO &io = ImGui::GetIO();

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings |
                                  ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav |
                                  ImGuiWindowFlags_NoMove;

  ImGui::SetNextWindowPos({0.f, 0.f}, ImGuiCond_Always, {0.f, 0.f});
  ImGui::SetNextWindowBgAlpha(0.5f);

  if (ImGui::Begin("Example: Simple overlay", nullptr, window_flags))
  {
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::SameLine();

    if (ImGui::Button("Erase and start new"))
      ImGui::OpenPopup("New?");
    ImGui::SameLine();

    if (ImGui::Button("Screenshot"))
      hesiod::gui::save_screenshot("screenshot.png");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("New?"))
    {
      // TODO quick and dirty, should eventually disappear
      static hmap::Vec2<int> gui_shape = hmap::Vec2<int>(1024, 1024);
      static hmap::Vec2<int> gui_tiling = hmap::Vec2<int>(4, 4);
      static float           gui_overlap = 0.25f;

      if (ImGui::Button("256 x 256"))
        gui_shape = {256, 256};
      ImGui::SameLine();

      if (ImGui::Button("512 x 512"))
        gui_shape = {512, 512};
      ImGui::SameLine();

      if (ImGui::Button("1024 x 1024"))
        gui_shape = {1024, 1024};
      ImGui::SameLine();

      if (ImGui::Button("2048 x 2048"))
        gui_shape = {2048, 2048};
      ImGui::SameLine();

      if (ImGui::Button("4096 x 4096"))
        gui_shape = {4096, 4096};

      ImGui::InputInt("tiling.x", &gui_tiling.x);
      ImGui::InputInt("tiling.y", &gui_tiling.y);

      ImGui::SliderFloat("overlap", &gui_overlap, 0.f, 0.5f, "%.2f");

      if (ImGui::Button("Ok"))
      {
        view_tree.clear();
        view_tree.set_sto(gui_shape, gui_tiling, gui_overlap);
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();

      if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }
  }
  ImGui::End();
}

} // namespace hesiod::gui
