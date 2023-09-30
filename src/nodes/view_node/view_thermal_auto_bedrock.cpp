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

ViewThermalAutoBedrock::ViewThermalAutoBedrock(std::string id)
    : ViewNode(), hesiod::cnode::ThermalAutoBedrock(id)
{
  LOG_DEBUG("ViewThermalAutoBedrock::ViewThermalAutoBedrock()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewThermalAutoBedrock::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("talus_global", &this->talus_global, 0.f, 4.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("iterations", &this->iterations, 1, 200);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewThermalAutoBedrock::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("talus_global", this->talus_global));
  ar(cereal::make_nvp("iterations", this->iterations));
}

void ViewThermalAutoBedrock::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("talus_global", this->talus_global));
  ar(cereal::make_nvp("iterations", this->iterations));
}

} // namespace hesiod::vnode
