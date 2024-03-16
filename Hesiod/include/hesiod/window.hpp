#pragma once
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "hesiod/gui.hpp"

namespace hesiod::window
{

class WindowPlatformBase
{
public:
  virtual ~WindowPlatformBase() = default;
  virtual bool initialize()
  {
    return false;
  };
  virtual bool shutdown()
  {
    return false;
  }
  // Will block the thread until the window is being closed!
  virtual bool run()
  {
    return false;
  }

  virtual bool set_title(std::string /* title */)
  {

    return false;
  }
  // width can be null; height can be null;
  virtual bool set_size(int /* width */, int /* height */)
  {
    return false;
  }

  virtual bool stop_running()
  {
    return false;
  }
  virtual bool is_running()
  {
    return false;
  }

  virtual std::string get_title()
  {
    return "Not implemented";
  }
  virtual bool get_size(int * /*width*/, int * /*height*/)
  {
    return false;
  }

  virtual gui::WindowManager *get_window_manager()
  {
    return nullptr;
  }
};

class WindowImplemented : public WindowPlatformBase
{
public:
  WindowImplemented(std::string window_title,
                    int         window_width,
                    int         window_height);
  ~WindowImplemented();

  bool initialize() override;
  bool shutdown() override;
  bool run() override;

  bool set_title(std::string title) override;
  bool set_size(int width, int height) override;

  bool is_running() override
  {
    return running;
  }

  std::string get_title() override;
  bool        get_size(int *width, int *height) override;

  gui::WindowManager *get_window_manager() override;

protected:
  void cleanup_internally();

private:
  std::string title;
  int         width;
  int         height;

  GLFWwindow *window;
  bool        running;

  ImColor clear_color;

  gui::WindowManager *window_manager;
};

} // namespace hesiod::window