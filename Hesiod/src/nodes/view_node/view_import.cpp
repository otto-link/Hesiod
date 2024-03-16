/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "ImGuiFileDialog.h"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"

namespace hesiod::vnode
{

void ViewImport::render_node_specific_content()
{
  if (ImGui::Button("reload!"))
    this->force_update();
}

} // namespace hesiod::vnode
