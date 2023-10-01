/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewClone::ViewClone(std::string id) : ViewNode(), hesiod::cnode::Clone(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

bool ViewClone::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (ImGui::Button("Add output"))
  {
    this->add_port(gnode::Port("thru##" + std::to_string(this->id_count++),
                               gnode::direction::out,
                               hesiod::cnode::dtype::dHeightMap));
    this->update_inner_bindings();
  }

  if (ImGui::Button("Remove unused outputs"))
    this->remove_unused_outputs();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewClone::serialize_save(cereal::JSONOutputArchive &)
{
  // empty
}

void ViewClone::serialize_load(cereal::JSONInputArchive &)
{
  // empty
}

} // namespace hesiod::vnode
