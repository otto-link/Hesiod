/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewKmeansClustering2::ViewKmeansClustering2(std::string id)
    : ViewNode(), hesiod::cnode::KmeansClustering2(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewKmeansClustering2::render_settings()
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

  ImGui::SliderInt("nclusters", &this->nclusters, 1, 8);
  has_changed |= this->trigger_update_after_edit();

  ImGui::TextUnformatted("Resolution");
  bool rbutton = false;
  rbutton |= ImGui::RadioButton("128 x 128", &this->shape_clustering_choice, 0);
  rbutton |= ImGui::RadioButton("256 x 256", &this->shape_clustering_choice, 1);
  rbutton |= ImGui::RadioButton("512 x 512", &this->shape_clustering_choice, 2);

  if (rbutton)
  {
    if (this->shape_clustering_choice == 0)
      this->shape_clustering = {128, 128};
    else if (this->shape_clustering_choice == 1)
      this->shape_clustering = {256, 256};
    else if (this->shape_clustering_choice == 2)
      this->shape_clustering = {512, 512};

    this->force_update();
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
