/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewKernel::ViewKernel(std::string id) : ViewNode(), hesiod::cnode::Kernel(id)
{
  LOG_DEBUG("hash_id: %d", this->hash_id);
  LOG_DEBUG("label: %s", this->label.c_str());
  this->set_p_control_node((gnode::Node *)this);
}

bool ViewKernel::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderInt("shape.x", &this->shape.x, 1, 256);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("shape.y", &this->shape.y, 1, 256);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::listbox_map_enum(this->kernel_map, this->kernel, 128.f))
  {
    this->force_update();
    has_changed = true;
  }

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

void ViewKernel::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("shape.x", this->shape.x));
  ar(cereal::make_nvp("shape.y", this->shape.y));
  ar(cereal::make_nvp("normalized", this->normalized));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
  ar(cereal::make_nvp("kernel", this->kernel));
}

void ViewKernel::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("shape.x", this->shape.x));
  ar(cereal::make_nvp("shape.y", this->shape.y));
  ar(cereal::make_nvp("normalized", this->normalized));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
  ar(cereal::make_nvp("kernel", this->kernel));
}

} // namespace hesiod::vnode
