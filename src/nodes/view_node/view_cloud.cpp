/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewCloud::ViewCloud(std::string id) : ViewNode(), hesiod::cnode::Cloud(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

// void ViewCloud::post_control_node_update()
// {
//   hmap::HeightMap *p_h = (hmap::HeightMap *)this->get_p_data("input");

//   this->log = "address: " + std::to_string((unsigned long long)(void **)p_h)
//   +
//               "\n";
//   this->log += "min: " + std::to_string(p_h->min()) + "\n";
//   this->log += "max: " + std::to_string(p_h->max()) + "\n";
//   this->log += "shape: {" + std::to_string(p_h->shape.x) + ", " +
//                std::to_string(p_h->shape.y) + "}\n";
//   this->log += "tiling: {" + std::to_string(p_h->tiling.x) + ", " +
//                std::to_string(p_h->tiling.y) + "}\n";
//   this->log += "overlap: " + std::to_string(p_h->overlap) + "\n";
//   // this->log += "" + std::to_string() + "\n";

//   if (this->auto_export_png)
//     this->export_to_png();
// }

bool ViewCloud::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (hesiod::gui::canvas_point_editor(this->value_out))
    this->force_update();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
