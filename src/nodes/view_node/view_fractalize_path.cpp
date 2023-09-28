/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewFractalizePath::ViewFractalizePath(std::string id)
    : ViewNode(), hesiod::cnode::FractalizePath(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

void ViewFractalizePath::render_node_specific_content()
{
  ImGui::Checkbox("Preview", &this->show_preview);

  if (this->show_preview)
    hesiod::gui::canvas_path(this->value_out, this->node_width);
}

bool ViewFractalizePath::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  ImGui::SliderInt("iterations", &this->iterations, 1, 8);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("sigma", &this->sigma, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("persistence", &this->persistence, 0.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("orientation", &this->orientation, 0, 1);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewFractalizePath::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("sigma", this->sigma));
  ar(cereal::make_nvp("orientation", this->orientation));
  ar(cereal::make_nvp("persistence", this->persistence));
}

void ViewFractalizePath::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("sigma", this->sigma));
  ar(cereal::make_nvp("orientation", this->orientation));
  ar(cereal::make_nvp("persistence", this->persistence));
}

} // namespace hesiod::vnode
