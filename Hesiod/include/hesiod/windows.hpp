#pragma once
#include <queue>
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "hesiod/shortcuts.hpp"

namespace hesiod::gui

{

// forward declarations
class WindowManager;

class Window : public ShortcutGroupElement
{
public:
  Window();
  virtual ~Window() = default;
  virtual bool initialize_window()
  {
    return true;
  }
  bool         render();
  virtual bool render_window_content();
  bool         add_shortcuts();
  bool         remove_shortcuts();

protected:
  friend class WindowManager;

  std::string                            title;
  ImGuiWindowFlags                       flags;
  WindowManager                         *p_parent_window_manager;
  std::vector<std::unique_ptr<Shortcut>> shortcuts;
};

class WindowManager
{
public:
  using Tag = unsigned int;
  static constexpr Tag kInvalidTag = (Tag)-1;

  WindowManager();

  Tag     find_free_tag();
  bool    has_window(Tag tag);
  Window *get_window_ref_by_tag(Tag tag);
  bool    remove_window(Tag tag);
  bool    remove_all_windows();

  Tag  add_window_with_tag(Tag tag, std::unique_ptr<Window> p_window);
  Tag  add_window(std::unique_ptr<Window> p_window);
  bool do_delete_queue();
  bool render_windows();

  void handle_input(int key, int scancode, int action, int modifiers);

  ShortcutsManager *get_shortcuts_manager_ref();

private:
  std::map<Tag, std::unique_ptr<Window>> windows;
  std::queue<Tag>                        windows_delete_queue;
  Tag                                    tag_count;

  ShortcutsManager shortcuts_manager;
};

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