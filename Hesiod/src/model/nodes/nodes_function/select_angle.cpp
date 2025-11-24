/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_angle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("angle", "angle", 0.f, 0.f, 360.f);
  node.add_attr<FloatAttribute>("sigma", "sigma", 90.f, 0.f, 180.f);
  node.add_attr<FloatAttribute>("radius", "radius", 0.f, 0.f, 0.2f);

  // attribute(s) order
  node.set_attr_ordered_key({"angle", "sigma", "radius"});

  setup_post_process_heightmap_attributes(node);
}

void compute_select_angle_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int ir = (int)(node.get_attr<FloatAttribute>("radius") * p_out->shape.x);

    hmap::transform(*p_out,
                    *p_in,
                    [&node, &ir](hmap::Array &array)
                    {
                      return select_angle(array,
                                          node.get_attr<FloatAttribute>("angle"),
                                          node.get_attr<FloatAttribute>("sigma"),
                                          ir);
                    });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node, *p_out);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
