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

void setup_quilting_expand_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "expansion_ratio", 2.f, 1.f, 16.f);
  ADD_ATTR(FloatAttribute, "patch_width", 0.3f, 0.1f, 1.f);
  ADD_ATTR(FloatAttribute, "overlap", 0.9f, 0.05f, 0.95f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(BoolAttribute, "patch_flip", true);
  ADD_ATTR(BoolAttribute, "patch_rotate", true);
  ADD_ATTR(BoolAttribute, "patch_transpose", true);
  ADD_ATTR(FloatAttribute, "filter_width_ratio", 0.5f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"expansion_ratio",
                                "patch_width",
                                "overlap",
                                "seed",
                                "patch_flip",
                                "patch_rotate",
                                "patch_transpose",
                                "filter_width_ratio"});
}

void compute_quilting_expand_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET("patch_width", FloatAttribute) * p_out->shape.x));

    hmap::Vec2<int> patch_base_shape = hmap::Vec2<int>(ir, ir);

    // --- work on a single array (i.e. not-tiled algo)

    hmap::Array in_array = p_in->to_array();
    hmap::Array out_array = hmap::Array(p_out->shape);

    out_array = hmap::quilting_expand(in_array,
                                      GET("expansion_ratio", FloatAttribute),
                                      patch_base_shape,
                                      GET("overlap", FloatAttribute),
                                      GET("seed", SeedAttribute),
                                      {},   // no secondary arrays
                                      true, // keep_input_shape
                                      GET("patch_flip", BoolAttribute),
                                      GET("patch_rotate", BoolAttribute),
                                      GET("patch_transpose", BoolAttribute),
                                      GET("filter_width_ratio", FloatAttribute));

    p_out->from_array_interp_nearest(out_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
