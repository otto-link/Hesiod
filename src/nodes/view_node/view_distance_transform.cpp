/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewDistanceTransform::ViewDistanceTransform(std::string id)
    : ViewNode(), hesiod::cnode::DistanceTransform(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewDistanceTransform::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::Checkbox("reverse", &this->reverse);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::select_shape("shape",
                                this->shape_working,
                                this->value_out.shape))
    this->force_update();

  ImGui::Separator();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewDistanceTransform::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("shape_working.x", this->shape_working.x));
  ar(cereal::make_nvp("shape_working.y", this->shape_working.y));
  ar(cereal::make_nvp("reverse", this->reverse));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewDistanceTransform::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("shape_working.x", this->shape_working.x));
  ar(cereal::make_nvp("shape_working.y", this->shape_working.y));
  ar(cereal::make_nvp("reverse", this->reverse));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
