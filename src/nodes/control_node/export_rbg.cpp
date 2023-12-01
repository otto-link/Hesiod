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
  this->category = category_mapping.at(this->node_type);

  this->attr["auto_export"] = NEW_ATTR_BOOL(false);
  this->attr["fname"] = NEW_ATTR_FILENAME("export.png");

  this->attr_ordered_key = {"auto_export", "fname"};

  this->add_port(
      gnode::Port("RGB", gnode::direction::in, dtype::dHeightMapRGB));
}

void ExportRGB::compute()
{
  if (GET_ATTR_BOOL("auto_export"))
    this->write_file();
}

void ExportRGB::write_file()
{
  if (this->get_p_data("RGB"))
  {
    hmap::HeightMapRGB *p_h = (hmap::HeightMapRGB *)this->get_p_data("RGB");
    p_h->to_png_16bit(GET_ATTR_FILENAME("fname"));
  }
}

} // namespace hesiod::cnode
