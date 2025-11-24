/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mixer_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 3");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 4");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "t");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("gain_factor", "gain_factor", 1.f, 0.001f, 10.f);

  // attribute(s) order
  node.set_attr_ordered_key({"gain_factor"});

  setup_post_process_heightmap_attributes(node);
}

void compute_mixer_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("input 2");
  hmap::Heightmap *p_in3 = node.get_value_ref<hmap::Heightmap>("input 3");
  hmap::Heightmap *p_in4 = node.get_value_ref<hmap::Heightmap>("input 4");
  hmap::Heightmap *p_t = node.get_value_ref<hmap::Heightmap>("t");

  std::vector<hmap::Heightmap *> ptr_list = {};
  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size() && p_t)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        {p_out, p_in1, p_in2, p_in3, p_in4, p_t},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays.front();
          hmap::Array *pa_t = p_arrays.back();

          std::vector<const hmap::Array *> arrays = {};

          for (size_t k = 1; k < p_arrays.size() - 1; k++)
            if (p_arrays[k])
              arrays.push_back(p_arrays[k]);

          *pa_out = hmap::mixer(*pa_t,
                                arrays,
                                node.get_attr<FloatAttribute>("gain_factor"));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }
}

} // namespace hesiod
