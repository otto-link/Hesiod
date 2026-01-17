/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_rib_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "bedrock");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG2(node));

  // attribute(s)
  node.add_attr<IntAttribute>("iterations", "iterations", 10, 1, INT_MAX);
}

void compute_thermal_rib_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  // if (p_in)
  // {
  //   hmap::VirtualArray *p_bedrock = node.get_value_ref<hmap::VirtualArray>("bedrock");
  //   hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

  //   // copy the input heightmap
  //   *p_out = *p_in;

  //   hmap::for_each_tile(*p_out,
  //                       p_bedrock,
  //                       [&node](hmap::Array &h_out, hmap::Array *p_bedrock_array)
  //                       {
  //                         hmap::thermal_rib(h_out,
  //                                           node.get_attr<IntAttribute>("iterations"),
  //                                           p_bedrock_array);
  //                       });

  //   p_out->smooth_overlap_buffers();
  // }
}

} // namespace hesiod
