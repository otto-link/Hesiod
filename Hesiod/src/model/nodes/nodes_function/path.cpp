/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::OUT, "path");

  // attribute(s)
  ADD_ATTR(PathAttribute, "path");
  ADD_ATTR(BoolAttribute, "reverse", false);
}

void compute_path_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path *p_out = p_node->get_value_ref<hmap::Path>("path");
  *p_out = GET("path", PathAttribute);

  if (GET("reverse", BoolAttribute))
    p_out->reverse();

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
