/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{

ShortcutGroupId NodeSettings::get_element_shortcut_group_id()
{
  return "GroupNodeSettings" + this->get_unique_id();
}

bool NodeSettings::initialize()
{
  LOG_DEBUG("initializing NodeSettings window for ViewTree [%s]",
            p_vtree->id.c_str());

  this->title = "Node settings " + p_vtree->id;
  this->flags = ImGuiWindowFlags_MenuBar;

  return true;
}

bool NodeSettings::render_content()
{

  for (auto &node_hid : this->p_vtree->get_selected_node_hid())
  {
    std::string node_id =
        this->p_vtree->get_node_ref_by_hash_id(node_hid.Get())->id;

    hesiod::gui::draw_icon(hesiod::gui::square,
                           {12.f, 12.f},
                           this->p_vtree->get_node_color(node_id),
                           true);
    ImGui::SameLine();
    ImGui::TextUnformatted(this->p_vtree->get_node_type(node_id).c_str());
    
    this->p_vtree->render_settings(node_id);
    ImGui::Dummy(ImVec2(0.f, 16.f));
  }

  return true;
}

} // namespace hesiod::gui
