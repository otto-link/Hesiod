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

ViewStratifyMultiscale::ViewStratifyMultiscale(std::string id)
    : ViewNode(), hesiod::cnode::StratifyMultiscale(id)
{
  LOG_DEBUG("ViewStratifyMultiscale::ViewStratifyMultiscale()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewStratifyMultiscale::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  ImGui::DragInt("seed", &this->seed);
  has_changed |= this->trigger_update_after_edit();

  ImGui::TextUnformatted("strata_noise");
  if (hesiod::gui::drag_float_vector(this->strata_noise,
                                     false,
                                     false,
                                     0.f,
                                     1.f))
    this->force_update();

  ImGui::TextUnformatted("gamma_list");
  if (hesiod::gui::drag_float_vector(this->gamma_list,
                                     false,
                                     false,
                                     0.05f,
                                     4.f))
    this->force_update();

  ImGui::TextUnformatted("gamma_noise");
  if (hesiod::gui::drag_float_vector(this->gamma_noise, false, false, 0.f, 1.f))
    this->force_update();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewStratifyMultiscale::serialize_save(cereal::JSONOutputArchive &ar)
{
}

void ViewStratifyMultiscale::serialize_load(cereal::JSONInputArchive &ar)
{
}

} // namespace hesiod::vnode
