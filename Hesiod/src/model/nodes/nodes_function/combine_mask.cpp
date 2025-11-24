/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_combine_mask_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<EnumAttribute>("method",
                               "method",
                               enum_mappings.mask_combine_method_map,
                               "intersection");
}

void compute_combine_mask_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("input 2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

    int method = node.get_attr<EnumAttribute>("method");

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
}

} // namespace hesiod
