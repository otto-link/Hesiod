/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_wrinkle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "wrinkle_amplitude", 0.03f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "wrinkle_angle", 0.f, -180.f, 180.f);
  ADD_ATTR(node, FloatAttribute, "displacement_amplitude", 0.5f, 0.f, 2.f);
  ADD_ATTR(node, FloatAttribute, "radius", 0.f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "kw", 2.f, 0.f, FLT_MAX);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(node, FloatAttribute, "weight", 1.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"wrinkle_amplitude",
                             "wrinkle_angle",
                             "displacement_amplitude",
                             "radius",
                             "kw",
                             "seed",
                             "octaves",
                             "weight"});
}

void compute_wrinkle_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        *p_out,
        p_mask,
        [&node, &ir](hmap::Array &x, hmap::Vec4<float> bbox, hmap::Array *p_mask)
        {
          hmap::wrinkle(x,
                        GET(node, "wrinkle_amplitude", FloatAttribute),
                        p_mask,
                        GET(node, "wrinkle_angle", FloatAttribute),
                        GET(node, "displacement_amplitude", FloatAttribute),
                        ir,
                        GET(node, "kw", FloatAttribute),
                        GET(node, "seed", SeedAttribute),
                        GET(node, "octaves", IntAttribute),
                        GET(node, "weight", FloatAttribute),
                        bbox);
        });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
