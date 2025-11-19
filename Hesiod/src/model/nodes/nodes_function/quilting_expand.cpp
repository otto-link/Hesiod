/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/synthesis.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_quilting_expand_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "expansion_ratio", 2.f, 1.f, 16.f);
  ADD_ATTR(node, FloatAttribute, "patch_width", 0.3f, 0.1f, 1.f);
  ADD_ATTR(node, FloatAttribute, "overlap", 0.9f, 0.05f, 0.95f);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, BoolAttribute, "patch_flip", true);
  ADD_ATTR(node, BoolAttribute, "patch_rotate", true);
  ADD_ATTR(node, BoolAttribute, "patch_transpose", true);
  ADD_ATTR(node, FloatAttribute, "filter_width_ratio", 0.5f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"expansion_ratio",
                             "patch_width",
                             "overlap",
                             "seed",
                             "patch_flip",
                             "patch_rotate",
                             "patch_transpose",
                             "filter_width_ratio"});
}

void compute_quilting_expand_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1,
                      (int)(GET(node, "patch_width", FloatAttribute) * p_out->shape.x));

    hmap::Vec2<int> patch_base_shape = hmap::Vec2<int>(ir, ir);

    // --- work on a single array (i.e. not-tiled algo)

    hmap::Array in_array = p_in->to_array();
    hmap::Array out_array = hmap::Array(p_out->shape);

    out_array = hmap::quilting_expand(in_array,
                                      GET(node, "expansion_ratio", FloatAttribute),
                                      patch_base_shape,
                                      GET(node, "overlap", FloatAttribute),
                                      GET(node, "seed", SeedAttribute),
                                      {},   // no secondary arrays
                                      true, // keep_input_shape
                                      GET(node, "patch_flip", BoolAttribute),
                                      GET(node, "patch_rotate", BoolAttribute),
                                      GET(node, "patch_transpose", BoolAttribute),
                                      GET(node, "filter_width_ratio", FloatAttribute));

    p_out->from_array_interp_nearest(out_array);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
