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

bool ViewStratifyMultiscale::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  has_changed |=
      this->attr.at("seed")->get_ref<SeedAttribute>()->render_settings("seed");

  if (ImGui::Button("Size +1"))
  {
    this->attr.at("n_strata")->get_ref<VecIntAttribute>()->value.push_back(1);
    this->attr.at("strata_noise")
        ->get_ref<VecFloatAttribute>()
        ->value.push_back(0.f);
    this->attr.at("gamma_list")
        ->get_ref<VecFloatAttribute>()
        ->value.push_back(1.f);
    this->attr.at("gamma_noise")
        ->get_ref<VecFloatAttribute>()
        ->value.push_back(0.f);
    has_changed = true;
  }
  ImGui::SameLine();

  if (ImGui::Button("Size -1") &&
      this->attr.at("n_strata")->get_ref<VecIntAttribute>()->value.size() > 1)
  {
    this->attr.at("n_strata")->get_ref<VecIntAttribute>()->value.pop_back();
    this->attr.at("strata_noise")
        ->get_ref<VecFloatAttribute>()
        ->value.pop_back();
    this->attr.at("gamma_list")->get_ref<VecFloatAttribute>()->value.pop_back();
    this->attr.at("gamma_noise")
        ->get_ref<VecFloatAttribute>()
        ->value.pop_back();
    has_changed = true;
  }

  has_changed |= this->attr.at("n_strata")
                     ->get_ref<VecIntAttribute>()
                     ->render_settings("n_strata");
  has_changed |= this->attr.at("strata_noise")
                     ->get_ref<VecFloatAttribute>()
                     ->render_settings("strata_noise");
  has_changed |= this->attr.at("gamma_list")
                     ->get_ref<VecFloatAttribute>()
                     ->render_settings("gamma_list");
  has_changed |= this->attr.at("gamma_noise")
                     ->get_ref<VecFloatAttribute>()
                     ->render_settings("gamma_noise");

  has_changed |= this->render_settings_footer();

  if (has_changed)
    this->force_update();

  return has_changed;
}

} // namespace hesiod::vnode
