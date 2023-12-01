/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

void ViewCloud::render_node_specific_content()
{
  ImGui::Checkbox("Preview", &this->show_preview);

  if (this->show_preview)
    hesiod::gui::canvas_cloud(this->value_out, this->node_width);
}

} // namespace hesiod::vnode
