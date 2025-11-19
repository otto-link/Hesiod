/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/constants/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mix_texture_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture1");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture2");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture3");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture4");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  ADD_ATTR(node, BoolAttribute, "use_sqrt_avg", true);
  ADD_ATTR(node, BoolAttribute, "reset_output_alpha", true);

  // attribute(s) order
  node.set_attr_ordered_key({"use_sqrt_avg", "reset_output_alpha"});
}

void compute_mix_texture_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_in1 = node.get_value_ref<hmap::HeightmapRGBA>("texture1");
  hmap::HeightmapRGBA *p_in2 = node.get_value_ref<hmap::HeightmapRGBA>("texture2");
  hmap::HeightmapRGBA *p_in3 = node.get_value_ref<hmap::HeightmapRGBA>("texture3");
  hmap::HeightmapRGBA *p_in4 = node.get_value_ref<hmap::HeightmapRGBA>("texture4");
  hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  std::vector<hmap::HeightmapRGBA *> ptr_list = {};

  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size())
  {
    *p_out = mix_heightmap_rgba(ptr_list, GET(node, "use_sqrt_avg", BoolAttribute));

    if (GET(node, "reset_output_alpha", BoolAttribute))
      p_out->set_alpha(1.f);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
