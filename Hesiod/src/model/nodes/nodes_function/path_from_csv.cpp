/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_from_csv_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::OUT, "path");

  // attribute(s)
  ADD_ATTR(node,
           FilenameAttribute,
           "fname",
           std::filesystem::path(""),
           "CSV files (*.csv)",
           false);

  // attribute(s) order
  node.set_attr_ordered_key({"fname"});
}

void compute_path_from_csv_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_out = node.get_value_ref<hmap::Path>("path");

  const std::string fname = GET(node, "fname", FilenameAttribute).string();
  std::ifstream     f(fname.c_str());

  if (f.good())
  {
    hmap::Path path_tmp;
    bool       ret = path_tmp.from_csv(fname);
    if (ret)
      *p_out = path_tmp;
    else
      Logger::log()->error("compute_path_from_csv_node: could not parse CSV file {}",
                           fname);
  }
  else
  {
    Logger::log()->error("compute_path_from_csv_node: could not load CSV file {}", fname);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
