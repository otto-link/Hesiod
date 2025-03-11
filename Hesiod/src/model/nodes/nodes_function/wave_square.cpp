/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_wave_square_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "kw", 2.f, 0.01f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "angle", 0.f, 0.f, 180.f);
  ADD_ATTR(FloatAttribute, "phase_shift", 0.f, -1.f, 1.f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"kw", "angle", "phase_shift", "_SEPARATOR_", "inverse", "remap"});
}

void compute_wave_square_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::Heightmap *p_dr = p_node->get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  hmap::fill(*p_out,
             p_dr,
             [p_node](hmap::Vec2<int> shape, hmap::Vec4<float> bbox, hmap::Array *p_noise)
             {
               return hmap::wave_square(shape,
                                        GET("kw", FloatAttribute),
                                        GET("angle", FloatAttribute),
                                        GET("phase_shift", FloatAttribute),
                                        p_noise,
                                        nullptr,
                                        nullptr,
                                        bbox);
             });

  // post-process
  post_process_heightmap(p_node,
                         *p_out,

                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
