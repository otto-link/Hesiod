/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_plateau_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.01f, 0.5f, "radius");
  p_node->add_attr<FloatAttribute>("factor", 4.f, 0.01f, 10.f, "factor");
  p_node->add_attr<BoolAttribute>("GPU", true, "GPU");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "factor", "_SEPARATOR_", "GPU"});
}

void compute_plateau_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(*p_out,
                      p_mask,
                      [p_node, &ir](hmap::Array &x, hmap::Array *p_mask) {
                        hmap::gpu::plateau(x, p_mask, ir, GET("factor", FloatAttribute));
                      });
    }
    else
    {
      hmap::transform(*p_out,
                      p_mask,
                      [p_node, &ir](hmap::Array &x, hmap::Array *p_mask)
                      { hmap::plateau(x, p_mask, ir, GET("factor", FloatAttribute)); });
    }

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
