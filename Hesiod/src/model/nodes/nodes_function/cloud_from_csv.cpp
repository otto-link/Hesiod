/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/cloud.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_from_csv_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(FilenameAttribute,
           "fname",
           std::filesystem::path(""),
           "CSV files (*.csv)",
           false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname"});
}

void compute_cloud_from_csv_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

  const std::string fname = GET("fname", FilenameAttribute).string();
  std::ifstream     f(fname.c_str());

  if (f.good())
  {
    hmap::Cloud cloud_tmp;
    bool        ret = cloud_tmp.from_csv(fname);
    if (ret)
      *p_out = cloud_tmp;
    else
      LOG->error("compute_cloud_from_csv_node: could not parse CSV file {}", fname);
  }
  else
  {
    LOG->error("compute_cloud_from_csv_node: could not load CSV file {}", fname);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
