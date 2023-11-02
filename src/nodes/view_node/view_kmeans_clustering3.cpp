/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewKmeansClustering3::ViewKmeansClustering3(std::string id)
    : ViewNode(), hesiod::cnode::KmeansClustering3(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewKmeansClustering3::render_settings()
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

  ImGui::SliderInt("nclusters", &this->nclusters, 1, 16);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("weights.x", &this->weights.x, 0.01f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("weights.y", &this->weights.y, 0.01f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("weights.z", &this->weights.z, 0.01f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::select_shape("shape",
                                this->shape_clustering,
                                this->value_out.shape))
    this->force_update();

  ImGui::Checkbox("normalize_inputs", &this->normalize_inputs);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewKmeansClustering3::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("nclusters", this->nclusters));
  ar(cereal::make_nvp("weights.x", this->weights.x));
  ar(cereal::make_nvp("weights.y", this->weights.y));
  ar(cereal::make_nvp("weights.z", this->weights.z));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("shape_clustering.x", this->shape_clustering.x));
  ar(cereal::make_nvp("shape_clustering.y", this->shape_clustering.y));
  ar(cereal::make_nvp("normalize_inputs", this->normalize_inputs));
}

void ViewKmeansClustering3::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("nclusters", this->nclusters));
  ar(cereal::make_nvp("weights.x", this->weights.x));
  ar(cereal::make_nvp("weights.y", this->weights.y));
  ar(cereal::make_nvp("weights.z", this->weights.z));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("shape_clustering.x", this->shape_clustering.x));
  ar(cereal::make_nvp("shape_clustering.y", this->shape_clustering.y));
  ar(cereal::make_nvp("normalize_inputs", this->normalize_inputs));
}

} // namespace hesiod::vnode
