/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_shuffle_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Path>(gnode::PortType::OUT, "output");

  // attribute(s)
  ADD_ATTR(FloatAttribute, "dx", 0.f, -0.5f, 0.5f);
  ADD_ATTR(FloatAttribute, "dy", 0.f, -0.5f, 0.5f);
  ADD_ATTR(FloatAttribute, "dv", 0.f, -0.5f, 0.5f);
  ADD_ATTR(SeedAttribute, "seed");

  // attribute(s) order
  p_node->set_attr_ordered_key({"dx", "dy", "dv", "seed"});
}

void compute_path_shuffle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Path *p_in = p_node->get_value_ref<hmap::Path>("input");

  if (p_in)
  {
    hmap::Path *p_out = p_node->get_value_ref<hmap::Path>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 0)
      p_out->shuffle(GET("dx", FloatAttribute),
                     GET("dy", FloatAttribute),
                     GET("seed", SeedAttribute),
                     GET("dv", FloatAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
