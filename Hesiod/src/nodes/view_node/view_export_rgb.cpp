/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"

namespace hesiod::vnode
{

void ViewExportRGB::render_node_specific_content()
{
  if (ImGui::Button("export!"))
    this->write_file();
}

} // namespace hesiod::vnode
