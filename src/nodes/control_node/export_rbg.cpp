/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ExportRGB::ExportRGB(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ExportRGB::ExportRGB()");
  this->node_type = "ExportRGB";
  this->add_port(
      gnode::Port("RGB", gnode::direction::in, dtype::dHeightMapRGB));
  this->category = category_mapping.at(this->node_type);
}

void ExportRGB::compute()
{
  if (this->auto_export)
    this->write_file();
}

void ExportRGB::write_file()
{
  if (this->get_p_data("RGB"))
  {
    hmap::HeightMapRGB *p_h = (hmap::HeightMapRGB *)this->get_p_data("RGB");
    p_h->to_png_16bit(this->fname);
  }
}

} // namespace hesiod::cnode
