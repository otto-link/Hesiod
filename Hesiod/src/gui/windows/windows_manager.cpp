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

WindowManager::WindowManager()
{
}

WindowTag WindowManager::get_new_tag()
{
  this->tag_count++;
  return (WindowTag)(this->tag_count);
}

bool WindowManager::has_window(WindowTag tag)
{
  return this->windows.contains(tag);
}

Window *WindowManager::get_window_ref_by_tag(WindowTag tag)
{
  if (this->has_window(tag) == false)
    return nullptr;

  return this->windows.at(tag).get();
}

bool WindowManager::remove_window(WindowTag tag)
{
  if (has_window(tag) == false)
    return false;

  this->windows_delete_queue.push_back(tag);

  return true;
}

bool WindowManager::remove_all_windows()
{
  this->windows.clear();
  return true;
}

WindowTag WindowManager::add_window_with_tag(WindowTag               tag,
                                             std::unique_ptr<Window> p_window)
{
  if (this->has_window(tag))
  {
    return WindowManager::kInvalidTag;
  }

  p_window->p_parent_window_manager = this;
  p_window->set_tag(tag);
  p_window->initialize();
  p_window->add_shortcuts();

  this->windows.emplace(tag, std::move(p_window));

  return tag;
}

WindowTag WindowManager::add_window(std::unique_ptr<Window> p_window)
{
  WindowTag tag = this->get_new_tag();

  return this->add_window_with_tag(tag, std::move(p_window));
}

bool WindowManager::do_delete_queue()
{
  for (auto &tag : this->windows_delete_queue)
    this->windows.erase(tag);

  this->windows_delete_queue.clear();

  return true;
}

bool WindowManager::render_windows()
{
  this->do_delete_queue();

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
