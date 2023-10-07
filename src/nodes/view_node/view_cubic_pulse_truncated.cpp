/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewCubicPulseTruncated::ViewCubicPulseTruncated(std::string id)
    : ViewNode(), hesiod::cnode::CubicPulseTruncated(id)
{
  LOG_DEBUG("hash_id: %d", this->hash_id);
  LOG_DEBUG("label: %s", this->label.c_str());
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewCubicPulseTruncated::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  int ir = (this->shape.x - 1) / 2;
  if (ImGui::SliderInt("ir", &ir, 1, 256))
  {
    this->shape.x = 2 * ir + 1;
    this->shape.y = this->shape.x;
  }
  has_changed |= this->trigger_update_after_edit();

  float alpha = this->angle / 180.f * M_PI;
  ImGui::SliderAngle("angle", &alpha, -90.f, 90.f);
  this->angle = alpha / M_PI * 180.f;
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("slant_ratio", &this->slant_ratio, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("normalized", &this->normalized);
  has_changed |= this->trigger_update_after_edit();

  if (!this->normalized)
    if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
    {
      this->force_update();
      has_changed = true;
    }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewCubicPulseTruncated::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("shape.x", this->shape.x));
  ar(cereal::make_nvp("shape.y", this->shape.y));
  ar(cereal::make_nvp("slant_ratio", this->slant_ratio));
  ar(cereal::make_nvp("angle", this->angle));
}

void ViewCubicPulseTruncated::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("shape.x", this->shape.x));
  ar(cereal::make_nvp("shape.y", this->shape.y));
  ar(cereal::make_nvp("slant_ratio", this->slant_ratio));
  ar(cereal::make_nvp("angle", this->angle));
}

} // namespace hesiod::vnode
