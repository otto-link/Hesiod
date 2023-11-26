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
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::StratifyMultiscale(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
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

  // TODO add / remove layer of strata

  if (ImGui::Button("Size +1"))
  {
    this->n_strata.push_back(1);
    this->strata_noise.push_back(0.f);
    this->gamma_list.push_back(1.f);
    this->gamma_noise.push_back(0.f);
    this->force_update();
  }
  ImGui::SameLine();

  if (ImGui::Button("Size -1") && this->n_strata.size() > 1)
  {
    this->n_strata.pop_back();
    this->strata_noise.pop_back();
    this->gamma_list.pop_back();
    this->gamma_noise.pop_back();
    this->force_update();
  }

  ImGui::TextUnformatted("n_strata");
  for (size_t k = 0; k < this->n_strata.size(); k++)
  {
    ImGui::SliderInt(("##n_strata" + std::to_string(k)).c_str(),
                     &(this->n_strata[k]),
                     1,
                     8);
    has_changed |= this->trigger_update_after_edit();
  }

  ImGui::TextUnformatted("strata_noise");
  if (hesiod::gui::drag_float_vector(this->strata_noise,
                                     false,
                                     false,
                                     0.f,
                                     1.f,
                                     true))
    this->force_update();

  ImGui::TextUnformatted("gamma_list");
  if (hesiod::gui::drag_float_vector(this->gamma_list,
                                     false,
                                     false,
                                     0.05f,
                                     4.f,
                                     true))
    this->force_update();

  ImGui::TextUnformatted("gamma_noise");
  if (hesiod::gui::drag_float_vector(this->gamma_noise,
                                     false,
                                     false,
                                     0.f,
                                     1.f,
                                     true))
    this->force_update();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
