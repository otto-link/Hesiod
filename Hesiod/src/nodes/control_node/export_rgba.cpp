/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ExportRGBA::ExportRGBA(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ExportRGBA::ExportRGBA()");
  this->node_type = "ExportRGBA";
  this->category = category_mapping.at(this->node_type);

  this->attr["auto_export"] = NEW_ATTR_BOOL(false);
  this->attr["fname"] = NEW_ATTR_FILENAME("export.png");

  this->attr_ordered_key = {"auto_export", "fname"};

  this->add_port(
      gnode::Port("RGBA", gnode::direction::in, dtype::dHeightMapRGBA));
}

void ExportRGBA::compute()
{
  if (GET_ATTR_BOOL("auto_export"))
    this->write_file();
}

void ExportRGBA::write_file()
{
  if (this->get_p_data("RGBA"))
  {
    hmap::HeightMapRGBA *p_h = (hmap::HeightMapRGBA *)this->get_p_data("RGBA");
    p_h->to_png_16bit(GET_ATTR_FILENAME("fname"));
  }
}

} // namespace hesiod::cnode
