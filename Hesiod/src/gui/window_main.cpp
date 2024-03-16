/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "ImCandy/candy.h"
#include "macrologger.h"

#include "hesiod/fonts.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{
void sGlfwErrorCallback(int error, const char *description)
{
  LOG_ERROR("GLFW Error %i (%s)", error, description ? description : "");
}

// Fix this
static bool sGLFWInitialized = false;

MainWindow::MainWindow(std::string window_title,
                       int         window_width,
                       int         window_height)
    : title(window_title), width(window_width), height(window_height),
      p_glfw_window(nullptr), running(false),
      clear_color(0.15f, 0.25f, 0.30f, 1.00f)
{
}

MainWindow::~MainWindow()
{
  if (this->is_running())
  {
    this->shutdown();
  }
}

bool MainWindow::initialize()
{
  if (sGLFWInitialized == false)
  {
    glfwSetErrorCallback(sGlfwErrorCallback);
    if (glfwInit() == false)
    {
      LOG_ERROR("glfwInit returned false.");
      return false;
    }
    LOG_INFO("glfwInit()");
    sGLFWInitialized = true;
  }

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

  this->p_glfw_window = glfwCreateWindow(this->width,
                                         this->height,
                                         this->title.c_str(),
                                         nullptr,
                                         nullptr);
  glfwMakeContextCurrent(this->p_glfw_window);
  glfwSwapInterval(1);

  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    LOG_ERROR("glewInit failed.");
    glfwTerminate();
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImFontConfig                config;
  std::vector<unsigned char> &font_data_vec = get_robot_reguar_ttf();
  config.OversampleH = 2;
  config.OversampleV = 2;
  config.FontDataOwnedByAtlas = false;
  io.Fonts->AddFontFromMemoryTTF(font_data_vec.data(),
                                 font_data_vec.size(),
                                 16.f,
                                 &config);

  ImGui::StyleColorsDark();
  ImCandy::Theme_Blender();

  glfwSetWindowUserPointer(this->p_glfw_window, this);
  glfwSetKeyCallback(
      this->p_glfw_window,
      [](GLFWwindow *p_window, int key, int scancode, int action, int modifiers)
      {
        MainWindow *instance = static_cast<MainWindow *>(
            glfwGetWindowUserPointer(p_window));
        if (action == GLFW_PRESS)
        {
          instance->get_window_manager_ref()->handle_input(key,
                                                           scancode,
                                                           action,
                                                           modifiers);
        }
      });

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(this->p_glfw_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  this->running = true;
  return this->running;
}

bool MainWindow::shutdown()
{
  if (this->running == false)
  {
    return true;
  }

  this->cleanup_internally();
  return true;
}

bool MainWindow::run()
{
  while (!glfwWindowShouldClose(this->p_glfw_window) && this->running)
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->get_window_manager_ref()
        ->get_shortcuts_manager_ref()
        ->set_input_blocked(ImGui::GetIO().WantTextInput);

    // --- GUI content
    window_manager.render_windows();

    // --- Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(this->p_glfw_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.Value.x * clear_color.Value.w,
                 clear_color.Value.y * clear_color.Value.w,
                 clear_color.Value.z * clear_color.Value.w,
                 clear_color.Value.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->p_glfw_window);
  }

  this->cleanup_internally();
  return true;
}

bool MainWindow::set_title(std::string window_title)
{
  if (this->p_glfw_window == nullptr)
    return false;

  this->title = window_title;
  glfwSetWindowTitle(this->p_glfw_window, this->title.c_str());

  return true;
}

bool MainWindow::set_size(int window_width, int window_height)
{
  if (this->p_glfw_window == nullptr)
    return false;

  glfwGetWindowSize(this->p_glfw_window, &this->width, &this->height);

  this->width = window_width ? window_width : this->width;
  this->height = window_height ? window_height : this->height;

  glfwSetWindowSize(this->p_glfw_window, this->width, this->height);

  return true;
}

std::string MainWindow::get_title()
{
  return this->title;
}

bool MainWindow::get_size(int &query_width, int &query_height)
{
  if (this->p_glfw_window == nullptr)
    return false;

  glfwGetWindowSize(this->p_glfw_window, &query_width, &query_height);

  // this->width = query_width;
  // this->height = query_height;

  return true;
}

WindowManager *MainWindow::get_window_manager_ref()
{
  return &(this->window_manager);
}

void MainWindow::cleanup_internally()
{
  if (running == false)
    return;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(this->p_glfw_window);
  glfwTerminate();

  this->p_glfw_window = nullptr;
  this->running = false;
}

} // namespace hesiod::gui
