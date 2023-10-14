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

ViewCloudToArrayInterp::ViewCloudToArrayInterp(std::string     id,
                                               hmap::Vec2<int> shape,
                                               hmap::Vec2<int> tiling,
                                               float           overlap)
    : ViewNode(), hesiod::cnode::CloudToArrayInterp(id, shape, tiling, overlap)
{
  LOG_DEBUG("ViewCloudToArrayInterp::ViewCloudToArrayInterp()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewCloudToArrayInterp::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::TextUnformatted("Interpolation method");
  bool rbutton = false;
  rbutton |= ImGui::RadioButton("linear", &this->interpolation_method, 0);
  rbutton |= ImGui::RadioButton("thinplate", &this->interpolation_method, 1);
  if (rbutton)
  {
    has_changed = true;
    this->force_update();
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewCloudToArrayInterp::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("interpolation_method", this->interpolation_method));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewCloudToArrayInterp::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("interpolation_method", this->interpolation_method));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
