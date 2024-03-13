/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "hesiod/gui.hpp"
#include "hesiod/view_tree.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "macrologger.h"

namespace hesiod::gui
{

// GuiRenderableWindowBase

GuiRenderableWindowBase::GuiRenderableWindowBase()
    : renderable_window_title(), renderable_window_flags()
{
}

bool GuiRenderableWindowBase::render_window()
{
  bool res = true;
  if (ImGui::Begin(renderable_window_title.data(),
                   nullptr,
                   renderable_window_flags))
  {
    res &= render_element_content();
  }
  ImGui::End();
  return res;
}

bool GuiRenderableWindowBase::render_element_content()
{
  ImGui::Text("render_element_content not implemented.");
  return true;
}

// GuiWindowManager

GuiWindowManager::GuiWindowManager()
    : windows(), windows_delete_queue(), tag_count()
{
}

GuiWindowManager::~GuiWindowManager()
{
  this->remove_all_windows();
}

GuiWindowManager::Tag GuiWindowManager::find_free_tag()
{
  Tag currentTag = tag_count;

  // Yes this is bad, and I know it.
  while (has_window(currentTag))
  {
    currentTag = (++tag_count);
  }

  return currentTag;
}

bool GuiWindowManager::has_window(Tag tag)
{
  return windows.count(tag) > 0;
}

GuiRenderableWindowBase *GuiWindowManager::get_window(Tag tag)
{
  if (has_window(tag) == false)
    return nullptr;

  return windows.at(tag);
}

bool GuiWindowManager::remove_window(Tag tag)
{
  if (has_window(tag) == false)
    return false;

  windows_delete_queue.push(tag);

  return true;
}

bool GuiWindowManager::remove_all_windows()
{
  std::map<Tag, GuiRenderableWindowBase *> copiedWindows = windows;
  windows.clear();

  for (std::map<Tag, GuiRenderableWindowBase *>::iterator currentIterator =
           copiedWindows.begin();
       currentIterator != copiedWindows.end();
       currentIterator++)
  {
    delete currentIterator->second;
  }

  return true;
}

GuiWindowManager::Tag GuiWindowManager::add_window_with_tag(
    Tag                      tag,
    GuiRenderableWindowBase *window)
{
  if (has_window(tag))
  {
    return GuiWindowManager::kInvalidTag;
  }

  windows.emplace(tag, window);
  return tag;
}

GuiWindowManager::Tag GuiWindowManager::add_window(
    GuiRenderableWindowBase *window)
{
  Tag tag = find_free_tag();
  windows.emplace(tag, window);
  return tag;
}

bool GuiWindowManager::do_delete_queue()
{
  while (windows_delete_queue.empty())
  {
    Tag tag = windows_delete_queue.front();
    windows_delete_queue.pop();

    if (has_window(tag))
    {
      delete windows.at(tag);
      windows.erase(tag);
    }
  }

  return true;
}

bool GuiWindowManager::render_windows()
{
  do_delete_queue();

  for (std::map<Tag, GuiRenderableWindowBase *>::iterator currentIterator =
           windows.begin();
       currentIterator != windows.end();
       currentIterator++)
  {
    currentIterator->second->render_window();
  }

  return true;
}

} // namespace hesiod::gui
