/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewSedimentDeposition::ViewSedimentDeposition(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::SedimentDeposition(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewSedimentDeposition::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("talus_global", &this->talus_global, 0.f, 8.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("max_deposition",
                     &this->max_deposition,
                     0.f,
                     0.1f,
                     "%.3f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("iterations", &this->iterations, 1, 200);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("thermal_subiterations",
                   &this->thermal_subiterations,
                   1,
                   200);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
