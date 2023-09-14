/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"
#include <cereal/archives/json.hpp>

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewFbmPerlin::ViewFbmPerlin(std::string     id,
                             hmap::Vec2<int> shape,
                             hmap::Vec2<int> tiling,
                             float           overlap)
    : ViewNode(), hesiod::cnode::FbmPerlin(id, shape, tiling, overlap)
{
  LOG_DEBUG("ViewFbmPerlin::ViewFbmPerlin()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view2d_port_id("output");
}

bool ViewFbmPerlin::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::AlignTextToFramePadding();
  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  ImGui::DragInt("seed", &this->seed);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::drag_float_vec2(this->kw, this->link_kxy))
  {
    has_changed = true;
    this->force_update();
  }

  ImGui::InputInt("octaves", &this->octaves);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("weight", &this->weight, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("persistence", &this->persistence, 0.01f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("lacunarity", &this->lacunarity, 0.01f, 4.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::Separator();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewFbmPerlin::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("kw.x", this->kw.x));
  ar(cereal::make_nvp("kw.y", this->kw.y));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("octaves", this->octaves));
  ar(cereal::make_nvp("weight", this->weight));
  ar(cereal::make_nvp("persistence", this->persistence));
  ar(cereal::make_nvp("lacunarity", this->lacunarity));
  ar(cereal::make_nvp("link_kxy", this->link_kxy));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewFbmPerlin::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("kw.x", this->kw.x));
  ar(cereal::make_nvp("kw.y", this->kw.y));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("octaves", this->octaves));
  ar(cereal::make_nvp("weight", this->weight));
  ar(cereal::make_nvp("persistence", this->persistence));
  ar(cereal::make_nvp("lacunarity", this->lacunarity));
  ar(cereal::make_nvp("link_kxy", this->link_kxy));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
