/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

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

} // namespace hesiod::vnode
