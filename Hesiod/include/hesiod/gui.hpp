/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <filesystem>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "hesiod/shortcuts.hpp"
#include "highmap.hpp"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace hesiod::gui
{

// Forward declaration for the parent of the Window
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
  virtual ~WindowManager();

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

  ShortcutsManager *p_shortcuts_manager;
};

} // namespace hesiod::gui
