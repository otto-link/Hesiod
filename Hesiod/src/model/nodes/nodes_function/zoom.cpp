/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_zoom_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("zoom_factor", "zoom_factor", 2.f, 1.f, 10.f);
  node.add_attr<BoolAttribute>("periodic", "periodic", false);
  node.add_attr<Vec2FloatAttribute>("center", "center");
  node.add_attr<BoolAttribute>("remap", "remap", false);

  // attribute(s) order
  node.set_attr_ordered_key({"zoom_factor", "periodic", "center", "remap"});
}

void compute_zoom_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::Array  dx_array, dy_array;
    hmap::Array *p_dx_array = nullptr, *p_dy_array = nullptr;

    if (p_dx)
    {
      dx_array = p_dx->to_array();
      p_dx_array = &dx_array;
    }

    if (p_dy)
    {
      dy_array = p_dy->to_array();
      p_dy_array = &dy_array;
    }

    z_array = hmap::zoom(z_array,
                         node.get_attr<FloatAttribute>("zoom_factor"),
                         node.get_attr<BoolAttribute>("periodic"),
                         node.get_attr<Vec2FloatAttribute>("center"),
                         p_dx_array,
                         p_dy_array);

    p_out->from_array_interp_nearest(z_array);

    if (node.get_attr<BoolAttribute>("remap"))
      p_out->remap();
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
