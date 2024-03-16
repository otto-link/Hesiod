/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"

namespace hesiod::vnode
{

bool ViewClone::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (ImGui::Button("Add output"))
  {
    std::string dummy = this->add_thru_port();
    this->update_inner_bindings();
  }

  if (ImGui::Button("Remove unused outputs"))
    this->remove_unused_outputs();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
