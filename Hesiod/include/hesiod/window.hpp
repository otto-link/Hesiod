#pragma once
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "hesiod/gui.hpp"

namespace hesiod::gui

{

class MainWindow
{
public:
  MainWindow(std::string window_title, int window_width, int window_height);

  ~MainWindow();

  bool initialize();

  bool shutdown();

  bool run();

  bool set_title(std::string title);

  bool set_size(int width, int height);

  bool is_running()
  {
    return this->running;
  }

  std::string get_title();

  bool get_size(int &width, int &height);

  WindowManager *get_window_manager_ref();

protected:
  void cleanup_internally();

private:
  std::string title;
  int         width;
  int         height;

  GLFWwindow *p_glfw_window;
  bool        running;

  ImColor clear_color;

  WindowManager window_manager;
};

} // namespace hesiod::gui