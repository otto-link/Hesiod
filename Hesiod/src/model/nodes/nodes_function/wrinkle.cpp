/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_wrinkle_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("wrinkle_amplitude",
                                   0.03f,
                                   0.f,
                                   0.2f,
                                   "wrinkle_amplitude");
  p_node->add_attr<FloatAttribute>("wrinkle_angle", 0.f, -180.f, 180.f, "wrinkle_angle");
  p_node->add_attr<FloatAttribute>("displacement_amplitude",
                                   0.5f,
                                   0.f,
                                   2.f,
                                   "displacement_amplitude");
  p_node->add_attr<FloatAttribute>("radius", 0.f, 0.f, 0.2f, "radius");
  p_node->add_attr<FloatAttribute>("kw", 2.f, 0.f, 8.f, "kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<IntAttribute>("octaves", 8, 0, 32, "Octaves");
  p_node->add_attr<FloatAttribute>("weight", 1.f, 0.f, 1.f, "Weight");

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

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

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
