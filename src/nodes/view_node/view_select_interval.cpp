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

ViewSelectInterval::ViewSelectInterval(std::string id)
    : ViewNode(), hesiod::cnode::SelectInterval(id)
{
  LOG_DEBUG("ViewSelectInterval::ViewSelectInterval()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewSelectInterval::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (hesiod::gui::slider_vmin_vmax(this->value1, this->value2))
  {
    this->force_update();
    has_changed = true;
  }

  ImGui::Separator();

  if (render_settings_mask(this->smoothing,
                           this->ir_smoothing,
                           this->normalize,
                           this->inverse))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewSelectInterval::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("value1", this->value1));
  ar(cereal::make_nvp("value2", this->value2));

  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("smoothing", this->smoothing));
  ar(cereal::make_nvp("ir_smoothing", this->ir_smoothing));
}

void ViewSelectInterval::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("value1", this->value1));
  ar(cereal::make_nvp("value2", this->value2));

  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("smoothing", this->smoothing));
  ar(cereal::make_nvp("ir_smoothing", this->ir_smoothing));
}

} // namespace hesiod::vnode
