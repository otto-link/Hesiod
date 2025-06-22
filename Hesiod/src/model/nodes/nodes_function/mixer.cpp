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

void setup_mixer_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 3");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 4");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "t");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
}

void compute_mixer_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("input 2");
  hmap::Heightmap *p_in3 = p_node->get_value_ref<hmap::Heightmap>("input 3");
  hmap::Heightmap *p_in4 = p_node->get_value_ref<hmap::Heightmap>("input 4");
  hmap::Heightmap *p_t = p_node->get_value_ref<hmap::Heightmap>("t");

  std::vector<hmap::Heightmap *> ptr_list = {};
  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size() && p_t)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        {p_out, p_in1, p_in2, p_in3, p_in4, p_t},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays.front();
          hmap::Array *pa_t = p_arrays.back();

          std::vector<const hmap::Array *> arrays = {};

          for (size_t k = 1; k < p_arrays.size() - 1; k++)
            if (p_arrays[k])
              arrays.push_back(p_arrays[k]);

          *pa_out = hmap::mixer(*pa_t, arrays);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
