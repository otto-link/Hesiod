/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Export::Export(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Export::Export()");
  this->node_type = "Export";
  this->category = category_mapping.at(this->node_type);

  this->attr["auto_export"] = NEW_ATTR_BOOL(false);
  this->attr["fname"] = NEW_ATTR_FILENAME("export.png");
  this->attr["export_format"] = NEW_ATTR_MAPENUM(this->format_map);

  this->attr_ordered_key = {"auto_export", "fname", "export_format"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
}

void Export::compute()
{
  if (GET_ATTR_BOOL("auto_export"))
    this->write_file();
}

void Export::write_file()
{
  if (this->get_p_data("input"))
  {
    hmap::HeightMap *p_h = (hmap::HeightMap *)this->get_p_data("input");

    int         export_format = GET_ATTR_MAPENUM("export_format");
    std::string fname = GET_ATTR_FILENAME("fname");

    if (export_format == hesiod::cnode::png8bit)
      p_h->to_array().to_png_grayscale_8bit(fname);

    else if (export_format == hesiod::cnode::png16bit)
      p_h->to_array().to_png_grayscale_16bit(fname);

    else if (export_format == hesiod::cnode::binary)
      p_h->to_array().to_file(fname);

    else if (export_format == hesiod::cnode::raw16bit)
      p_h->to_array().to_raw_16bit(fname);
  }
}

} // namespace hesiod::cnode
