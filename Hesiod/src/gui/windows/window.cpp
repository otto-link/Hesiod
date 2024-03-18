/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "macrologger.h"

#include "hesiod/shortcuts.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{

Window::Window() : title(), flags()
{
}

std::string Window::get_unique_id()
{
  return this->title + "_" + std::to_string((int)this->tag);
}

bool Window::render()
{
  bool res = true;

  std::string unique_title = this->title + "_" + this->get_unique_id();

  if (this->is_open &&
      ImGui::Begin(unique_title.c_str(), &this->is_open, this->flags))
  {
    if (ImGui::IsWindowFocused())
      this->p_parent_window_manager->get_shortcuts_manager_ref()
          ->set_focused_group_id(this->get_element_shortcut_group_id());

    res &= this->render_content();
  }
  else
    // the window destroys itself...
    this->get_window_manager_ref()->remove_window(this->tag);

  return res;
}

bool Window::add_shortcuts()
{
  for (auto &s : this->shortcuts)
  {
    this->p_parent_window_manager->get_shortcuts_manager_ref()->add_shortcut(
        std::move(s));
  }
  return true;
}

bool Window::remove_shortcuts()
{
  for (auto &s : shortcuts)
  {
    this->p_parent_window_manager->get_shortcuts_manager_ref()->remove_shortcut(
        s->get_id());
  }
  this->shortcuts.clear();
  return true;
}

} // namespace hesiod::gui
