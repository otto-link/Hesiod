/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_mixer_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input 1");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input 2");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input 3");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input 4");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "t");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);
}

void compute_mixer_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in1 = p_node->get_value_ref<hmap::HeightMap>("input 1");
  hmap::HeightMap *p_in2 = p_node->get_value_ref<hmap::HeightMap>("input 2");
  hmap::HeightMap *p_in3 = p_node->get_value_ref<hmap::HeightMap>("input 3");
  hmap::HeightMap *p_in4 = p_node->get_value_ref<hmap::HeightMap>("input 4");
  hmap::HeightMap *p_t = p_node->get_value_ref<hmap::HeightMap>("t");

  std::vector<hmap::HeightMap *> ptr_list = {};
  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size() && p_t)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    hmap::transform(*p_out,
                    p_in1,
                    p_in2,
                    p_in3,
                    p_in4,
                    p_t,
                    [](hmap::Array &out,
                       hmap::Array *p_a_in1,
                       hmap::Array *p_a_in2,
                       hmap::Array *p_a_in3,
                       hmap::Array *p_a_in4,
                       hmap::Array *p_a_t)
                    {
                      std::vector<hmap::Array *> arrays = {};

                      for (auto &ptr : {p_a_in1, p_a_in2, p_a_in3, p_a_in4})
                        if (ptr)
                          arrays.push_back(ptr);

                      out = hmap::mixer(*p_a_t, arrays);
                    });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
