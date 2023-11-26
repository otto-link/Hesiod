/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewKernel::ViewKernel(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Kernel(id)
{
  this->set_preview_port_id("output");
}

bool ViewKernel::render_settings()
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

} // namespace hesiod::vnode
