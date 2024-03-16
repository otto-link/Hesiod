/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <filesystem>
#include <map>
#include <mutex>
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

// Forward declaration for the parent of the GuiRenderableWindowBase
class GuiWindowManager;

class GuiRenderableElement
{
public:
  virtual ~GuiRenderableElement() = default;
  virtual bool render_element_content()
  {
    return false;
  }
};

class GuiRenderableWindowBase : public GuiRenderableElement,
                                public gui::GuiShortcutGroupElement
{
public:
  GuiRenderableWindowBase();
  virtual ~GuiRenderableWindowBase() = default;
  virtual bool initialize_window()
  {
    return true;
  }
  virtual bool render_window();
  bool         render_element_content() override;
  virtual bool add_window_shortcuts();
  virtual bool remove_window_shortcuts();

protected:
  friend class GuiWindowManager;

  std::string                           renderable_window_title;
  ImGuiWindowFlags                      renderable_window_flags;
  GuiWindowManager                     *renderable_window_manager_parent;
  std::vector<gui::GuiShortcut *> renderable_window_shortcuts;
};

class GuiWindowManager
{
public:
  using Tag = unsigned int;
  static constexpr Tag kInvalidTag = (Tag)-1;

  GuiWindowManager();
  virtual ~GuiWindowManager();

  virtual Tag                      find_free_tag();
  virtual bool                     has_window(Tag tag);
  virtual GuiRenderableWindowBase *get_window(Tag tag);
  virtual bool                     remove_window(Tag tag);
  virtual bool                     remove_all_windows();

  virtual Tag  add_window_with_tag(Tag tag, GuiRenderableWindowBase *window);
  virtual Tag  add_window(GuiRenderableWindowBase *window);
  virtual bool do_delete_queue();
  virtual bool render_windows();

  virtual void handle_input(int key, int scancode, int action, int modifiers);

  virtual gui::GuiShortcutsManager *get_shortcuts_manager();

private:
  std::map<Tag, GuiRenderableWindowBase *> windows;
  std::queue<Tag>                          windows_delete_queue;
  Tag                                      tag_count;

  gui::GuiShortcutsManager *shortcuts_manager;
};

} // namespace hesiod::gui
