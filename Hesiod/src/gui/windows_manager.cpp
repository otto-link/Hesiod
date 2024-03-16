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

namespace hesiod::gui
{

WindowManager::WindowManager()
{
}

WindowManager::Tag WindowManager::find_free_tag()
{
  Tag currentTag = this->tag_count;

  // Yes this is bad, and I know it.
  while (this->has_window(currentTag))
  {
    currentTag = (++this->tag_count);
  }

  return currentTag;
}

bool WindowManager::has_window(Tag tag)
{
  return this->windows.contains(tag);
}

Window *WindowManager::get_window_ref_by_tag(Tag tag)
{
  if (this->has_window(tag) == false)
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

  p_window->p_parent_window_manager = this;
  p_window->initialize_window();
  p_window->add_shortcuts();
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
  //     w->remove_shortcuts();
  //     delete w;
  //     windows.erase(tag);
  //   }
  // }

  return true;
}

bool WindowManager::render_windows()
{
  // do_delete_queue();

  for (auto &[tag, p_window] : this->windows)
    p_window->render();

  return true;
}

void WindowManager::handle_input(int key,
                                 int /* scancode */,
                                 int /* action */,
                                 int modifiers)
{
  this->shortcuts_manager.pass_and_check(key, modifiers);
}

gui::ShortcutsManager *WindowManager::get_shortcuts_manager_ref()
{
  return &(this->shortcuts_manager);
}

} // namespace hesiod::gui
