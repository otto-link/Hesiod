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

void setup_path_meanderize_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Path>(gnode::PortType::OUT, "output");

  // attribute(s)
  ADD_ATTR(FloatAttribute, "ratio", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "noise_ratio", 0.1f, 0.f, 1.f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(IntAttribute, "iterations", 2, 1, 8);
  ADD_ATTR(IntAttribute, "edge_divisions", 10, 1, 32);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"ratio", "noise_ratio", "seed", "iterations", "edge_divisions"});
}

void compute_path_meanderize_node(BaseNode *p_node)
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

    if (p_in->get_npoints() > 1)
      p_out->meanderize(GET("ratio", FloatAttribute),
                        GET("noise_ratio", FloatAttribute),
                        GET("seed", SeedAttribute),
                        GET("iterations", IntAttribute),
                        GET("edge_divisions", IntAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
