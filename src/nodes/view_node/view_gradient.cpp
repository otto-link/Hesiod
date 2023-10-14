/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGradient::ViewGradient(std::string id)
    : ViewNode(), hesiod::cnode::Gradient(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

bool ViewGradient::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("normalize", &this->normalize);
  has_changed |= this->trigger_update_after_edit();

  if (this->normalize)
  {
    ImGui::Text("dx");
    if (hesiod::gui::slider_vmin_vmax(this->vmin_x, this->vmax_x))
    {
      this->force_update();
      has_changed = true;
    }

    ImGui::Text("dy");
    if (hesiod::gui::slider_vmin_vmax(this->vmin_y, this->vmax_y))
    {
      this->force_update();
      has_changed = true;
    }
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewGradient::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("vmin_x", this->vmin_x));
  ar(cereal::make_nvp("vmax_x", this->vmax_x));
  ar(cereal::make_nvp("vmin_y", this->vmin_y));
  ar(cereal::make_nvp("vmax_y", this->vmax_y));
}

void ViewGradient::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("vmin_x", this->vmin_x));
  ar(cereal::make_nvp("vmax_x", this->vmax_x));
  ar(cereal::make_nvp("vmin_y", this->vmin_y));
  ar(cereal::make_nvp("vmax_y", this->vmax_y));
}

} // namespace hesiod::vnode
