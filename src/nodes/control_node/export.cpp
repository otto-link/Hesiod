/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Export::Export(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Export::Export()");
  this->node_type = "Export";
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->category = category_mapping.at(this->node_type);
}

void Export::compute()
{
  if (this->auto_export)
    this->write_file();
}

void Export::write_file()
{
  if (this->get_p_data("input"))
  {
    hmap::HeightMap *p_h = (hmap::HeightMap *)this->get_p_data("input");

    if (this->export_format == hesiod::cnode::png8bit)
      p_h->to_array().to_png(this->fname, hmap::cmap::gray);
  }
}

} // namespace hesiod::cnode
