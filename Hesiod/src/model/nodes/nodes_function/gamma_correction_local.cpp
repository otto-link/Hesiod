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

void setup_gamma_correction_local_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.01f, 0.2f, "radius");
  p_node->add_attr<FloatAttribute>("gamma", 2.f, 0.01f, 10.f, "gamma");
  p_node->add_attr<FloatAttribute>("k", 0.1f, 0.f, 0.5f, "k");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "gamma", "k"});
}

void compute_gamma_correction_local_node(BaseNode *p_node)
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

    float hmin = p_out->min();
    float hmax = p_out->max();
    p_out->remap(0.f, 1.f, hmin, hmax);

    hmap::transform(*p_out,
                    p_mask,
                    [p_node, &ir](hmap::Array &x, hmap::Array *p_mask)
                    {
                      hmap::gamma_correction_local(x,
                                                   GET("gamma", FloatAttribute),
                                                   ir,
                                                   p_mask,
                                                   GET("k", FloatAttribute));
                    });

    p_out->remap(hmin, hmax, 0.f, 1.f);
    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod