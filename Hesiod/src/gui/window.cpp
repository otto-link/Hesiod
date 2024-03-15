#include "hesiod/window.hpp"

#include "ImCandy/candy.h"
#include "hesiod/fonts.hpp"
#include "hesiod/gui.hpp"
#include "macrologger.h"
#include <vector>

namespace hesiod::window
{
void sGlfwErrorCallback(int error, const char *description)
{
  LOG_ERROR("GLFW Error %i (%s)", error, description ? description : "");
}

// Fix this
static bool sGLFWInitialized = false;

WindowImplemented::WindowImplemented(std::string window_title,
                                     int         window_width,
                                     int         window_height)
    : title(window_title), width(window_width), height(window_height),
      window(nullptr), running(false), clear_color(0.15f, 0.25f, 0.30f, 1.00f)
{
}

WindowImplemented::~WindowImplemented()
{
  if (is_running())
  {
    shutdown();
  }
}

bool WindowImplemented::initialize()
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
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
  glfwMakeContextCurrent(window);
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
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImFontConfig                config;
  std::vector<unsigned char> &fontDataVec = get_robot_reguar_ttf();
  config.OversampleH = 2;
  config.OversampleV = 2;
  config.FontDataOwnedByAtlas = false;
  io.Fonts->AddFontFromMemoryTTF(fontDataVec.data(),
                                 fontDataVec.size(),
                                 16.f,
                                 &config);

  ImGui::StyleColorsDark();
  ImCandy::Theme_Blender();

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(
      window,
      [](GLFWwindow *window, int key, int scancode, int action, int modifiers)
      {
        WindowImplemented *instance = static_cast<WindowImplemented *>(
            glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS)
        {
          instance->get_window_manager()->handle_input(key,
                                                       scancode,
                                                       action,
                                                       modifiers);
        }
      });

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  window_manager = new gui::GuiWindowManager();

  running = true;
  return running;
}

bool WindowImplemented::shutdown()
{
  if (running == false)
  {
    return true;
  }

  cleanup_internally();
  return true;
}

bool WindowImplemented::run()
{
  while (!glfwWindowShouldClose(window) && running)
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    get_window_manager()->get_shortcuts_manager()->set_input_blocked(
        ImGui::GetIO().WantTextInput);

    // --- GUI content
    window_manager->render_windows();
    // --- Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.Value.x * clear_color.Value.w,
                 clear_color.Value.y * clear_color.Value.w,
                 clear_color.Value.z * clear_color.Value.w,
                 clear_color.Value.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  cleanup_internally();
  return true;
}

bool WindowImplemented::set_title(std::string window_title)
{
  if (window == nullptr)
    return false;

  title = window_title;
  glfwSetWindowTitle(window, window_title.data());

  return true;
}

bool WindowImplemented::set_size(int window_width, int window_height)
{
  if (window == nullptr)
    return false;

  glfwGetWindowSize(window, &width, &height);

  width = window_width ? window_width : width;
  height = window_height ? window_height : height;

  glfwSetWindowSize(window, width, height);

  return true;
}

std::string WindowImplemented::get_title()
{
  return title;
}

bool WindowImplemented::get_size(int *width, int *height)
{
  if (window == nullptr)
    return false;

  glfwGetWindowSize(window, width, height);
  return true;
}

gui::GuiWindowManager *WindowImplemented::get_window_manager()
{
  return window_manager;
}

void WindowImplemented::cleanup_internally()
{
  if (running == false)
    return;

  if (window_manager)
  {
    delete window_manager;
    window_manager = nullptr;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  window = nullptr;
  running = false;
}
} // namespace hesiod::window