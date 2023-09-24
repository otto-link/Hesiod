/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

#include "hesiod/gui.hpp"

namespace hesiod::gui
{

void glfw_error_callback(int error, const char *description)
{
  std::cout << "GLFW Error " << error << " " << description << std::endl;
}

GLFWwindow *init_gui(int width, int height, std::string window_title)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return nullptr;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  GLFWwindow *window =
      glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
  }

  //
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return window;
}

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
  hmap::write_png_8bit(fname, img, hmap::Vec2<int>(width, height));
}

void main_dock()
{
  ImGuiIO &io = ImGui::GetIO();

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings |
                                  ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav |
                                  ImGuiWindowFlags_NoMove;

  ImGui::SetNextWindowPos({0.f, 0.f}, ImGuiCond_Always, {0.f, 0.f});
  ImGui::SetNextWindowBgAlpha(0.35f);

  if (ImGui::Begin("Example: Simple overlay", nullptr, window_flags))
  {
    ImGui::Text("FPS: %.1f", io.Framerate);
  }
  ImGui::End();
}

} // namespace hesiod::gui
