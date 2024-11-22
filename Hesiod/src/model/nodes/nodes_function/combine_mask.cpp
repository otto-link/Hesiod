/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_combine_mask_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("method",
                                     "intersection",
                                     mask_combine_method_map,
                                     "method");
}

void compute_combine_mask_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("input 2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

    int method = GET("method", MapEnumAttribute);

    switch (method)
    {
    case MaskCombineMethod::UNION:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::maximum(a1, a2); };
      break;

    case MaskCombineMethod::INTERSECTION:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::minimum(a1, a2); };
      break;

    case MaskCombineMethod::EXCLUSION:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      {
        m = a1 - a2;
        hmap::clamp_min(m, 0.f);
      };
      break;
    }

    hmap::transform(*p_out, *p_in1, *p_in2, lambda);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
