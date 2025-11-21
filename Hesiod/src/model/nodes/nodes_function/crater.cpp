/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_crater_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.1f, 0.01f, 1.f);

  node.add_attr<FloatAttribute>("depth", "depth", 0.2f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("lip_decay", "lip_decay", 0.1f, 0.01f, 1.f);

  node.add_attr<FloatAttribute>("lip_height_ratio", "lip_height_ratio", 2.f, 0.01f, 10.f);

  node.add_attr<Vec2FloatAttribute>("center", "center");

  node.add_attr<BoolAttribute>("inverse", "inverse", false);

  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"radius",
                             "depth",
                             "lip_decay",
                             "lip_height_ratio",
                             "center",
                             "_SEPARATOR_",
                             "inverse",
                             "remap"});
}

void compute_crater_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             p_ctrl,
             [&node](hmap::Vec2<int>   shape,
                     hmap::Vec4<float> bbox,
                     hmap::Array      *p_noise_x,
                     hmap::Array      *p_noise_y,
                     hmap::Array      *p_ctrl)
             {
               return hmap::crater(shape,
                                   node.get_attr<FloatAttribute>("radius"),
                                   node.get_attr<FloatAttribute>("depth"),
                                   node.get_attr<FloatAttribute>("lip_decay"),
                                   node.get_attr<FloatAttribute>("lip_height_ratio"),
                                   p_ctrl,
                                   p_noise_x,
                                   p_noise_y,
                                   node.get_attr<Vec2FloatAttribute>("center"),
                                   bbox);
             });

  // post-process
  post_process_heightmap(node,
                         *p_out,
                         node.get_attr<BoolAttribute>("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                         node.get_attr<RangeAttribute>("remap"));

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
