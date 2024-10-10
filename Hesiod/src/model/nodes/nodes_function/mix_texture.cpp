/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_mix_texture_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture1");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture2");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture3");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture4");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "texture", CONFIG);

  // attribute(s)
  p_node->add_attr<BoolAttribute>("use_sqrt_avg", true, "use_sqrt_avg");
  p_node->add_attr<BoolAttribute>("reset_output_alpha", true, "reset_output_alpha");

  // attribute(s) order
  p_node->set_attr_ordered_key({"use_sqrt_avg", "reset_output_alpha"});
}

void compute_mix_texture_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMapRGBA *p_in1 = p_node->get_value_ref<hmap::HeightMapRGBA>("texture1");
  hmap::HeightMapRGBA *p_in2 = p_node->get_value_ref<hmap::HeightMapRGBA>("texture2");
  hmap::HeightMapRGBA *p_in3 = p_node->get_value_ref<hmap::HeightMapRGBA>("texture3");
  hmap::HeightMapRGBA *p_in4 = p_node->get_value_ref<hmap::HeightMapRGBA>("texture4");
  hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>("texture");

  std::vector<hmap::HeightMapRGBA *> ptr_list = {};

  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size())
  {
    *p_out = mix_heightmap_rgba(ptr_list, GET("use_sqrt_avg", BoolAttribute));

    if (GET("reset_output_alpha", BoolAttribute))
      p_out->set_alpha(1.f);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod