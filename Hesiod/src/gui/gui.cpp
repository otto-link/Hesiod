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

#include "hesiod/gui.hpp"
#include "hesiod/shortcuts.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::gui
{

// Window

Window::Window() : renderable_window_title(), renderable_window_flags()
{
}

bool Window::render_window()
{
  bool res = true;
  if (ImGui::Begin(renderable_window_title.data(),
                   nullptr,
                   renderable_window_flags))
  {
    if (ImGui::IsWindowFocused())
    {
      this->renderable_window_manager_parent->get_shortcuts_manager()
          ->set_focused_group_id(this->get_element_shortcut_group_id());
    }

    res &= render_element_content();
  }
  ImGui::End();
  return res;
}

bool Window::render_element_content()
{
  ImGui::Text("render_element_content not implemented.");
  return true;
}

bool Window::add_window_shortcuts()
{
  for (auto &s : renderable_window_shortcuts)
  {
    renderable_window_manager_parent->get_shortcuts_manager()->add_shortcut(
        std::move(s));
  }
  return true;
}

bool Window::remove_window_shortcuts()
{
  for (auto &s : renderable_window_shortcuts)
  {
    renderable_window_manager_parent->get_shortcuts_manager()->remove_shortcut(
        s->get_id());
  }
  renderable_window_shortcuts.clear();
  return true;
}

// WindowManager

WindowManager::WindowManager()
    : windows(), windows_delete_queue(), tag_count(),
      shortcuts_manager(new gui::ShortcutsManager())
{
}

WindowManager::~WindowManager()
{
  this->remove_all_windows();

  if (shortcuts_manager)
  {
    delete shortcuts_manager;
    shortcuts_manager = nullptr;
  }
}

WindowManager::Tag WindowManager::find_free_tag()
{
  Tag currentTag = tag_count;

  // Yes this is bad, and I know it.
  while (has_window(currentTag))
  {
    currentTag = (++tag_count);
  }

  return currentTag;
}

bool WindowManager::has_window(Tag tag)
{
  return windows.count(tag) > 0;
}

Window *WindowManager::get_window_ref_by_tag(Tag tag)
{
  if (has_window(tag) == false)
    return nullptr;

  return this->windows.at(tag).get();
}

bool WindowManager::remove_window(Tag tag)
{
  if (has_window(tag) == false)
    return false;

  windows_delete_queue.push(tag);

  return true;
}

bool WindowManager::remove_all_windows()
{
  this->windows.clear();
  return true;
}

WindowManager::Tag WindowManager::add_window_with_tag(
    Tag                     tag,
    std::unique_ptr<Window> p_window)
{
  if (this->has_window(tag))
  {
    return WindowManager::kInvalidTag;
  }

  p_window->renderable_window_manager_parent = this;
  p_window->initialize_window();
  p_window->add_window_shortcuts();
  this->windows.emplace(tag, std::move(p_window));
  return tag;
}

WindowManager::Tag WindowManager::add_window(std::unique_ptr<Window> p_window)
{
  Tag tag = this->find_free_tag();
  return this->add_window_with_tag(tag, std::move(p_window));
}

bool WindowManager::do_delete_queue()
{
  // while (windows_delete_queue.empty())
  // {
  //   Tag tag = windows_delete_queue.front();
  //   windows_delete_queue.pop();

  //   if (has_window(tag))
  //   {
  //     Window *w = windows.at(tag);
  //     w->remove_window_shortcuts();
  //     delete w;
  //     windows.erase(tag);
  //   }
  // }

  return true;
}

bool WindowManager::render_windows()
{
  do_delete_queue();

  for (auto &[tag, p_window] : this->windows)
    p_window->render_window();

  return true;
}

void WindowManager::handle_input(int key,
                                 int /* scancode */,
                                 int /* action */,
                                 int modifiers)
{
  shortcuts_manager->pass_and_check(key, modifiers);
}

gui::ShortcutsManager *WindowManager::get_shortcuts_manager()
{
  return shortcuts_manager;
}

} // namespace hesiod::gui
