/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_unsphericity_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.01f, 0.f, 0.2f, "radius");

  p_node->add_attr<BoolAttribute>("remap", true, "remap");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<BoolAttribute>("smoothing", false, "smoothing");
  p_node->add_attr<FloatAttribute>("smoothing_radius",
                                   0.05f,
                                   0.f,
                                   0.2f,
                                   "smoothing_radius");
}

void compute_unsphericity_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    // zero radius accepted
    int ir = std::max(0, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(*p_out,
                    *p_in,
                    [&ir](hmap::Array &out, hmap::Array &in)
                    { out = hmap::unsphericity(in, ir); });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_out,
                           GET("inverse", BoolAttribute),
                           GET("smoothing", BoolAttribute),
                           GET("smoothing_radius", FloatAttribute),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET("remap", BoolAttribute),
                           {0.f, 1.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
