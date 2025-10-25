/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_wrinkle_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "wrinkle_amplitude", 0.03f, 0.f, 0.2f);
  ADD_ATTR(FloatAttribute, "wrinkle_angle", 0.f, -180.f, 180.f);
  ADD_ATTR(FloatAttribute, "displacement_amplitude", 0.5f, 0.f, 2.f);
  ADD_ATTR(FloatAttribute, "radius", 0.f, 0.f, 0.2f);
  ADD_ATTR(FloatAttribute, "kw", 2.f, 0.f, FLT_MAX);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(FloatAttribute, "weight", 1.f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"wrinkle_amplitude",
                                "wrinkle_angle",
                                "displacement_amplitude",
                                "radius",
                                "kw",
                                "seed",
                                "octaves",
                                "weight"});
}

void compute_wrinkle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        *p_out,
        p_mask,
        [p_node, &ir](hmap::Array &x, hmap::Vec4<float> bbox, hmap::Array *p_mask)
        {
          hmap::wrinkle(x,
                        GET("wrinkle_amplitude", FloatAttribute),
                        p_mask,
                        GET("wrinkle_angle", FloatAttribute),
                        GET("displacement_amplitude", FloatAttribute),
                        ir,
                        GET("kw", FloatAttribute),
                        GET("seed", SeedAttribute),
                        GET("octaves", IntAttribute),
                        GET("weight", FloatAttribute),
                        bbox);
        });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
