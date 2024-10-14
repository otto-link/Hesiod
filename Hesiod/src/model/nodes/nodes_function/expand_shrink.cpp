/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_expand_shrink_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("kernel", "cubic_pulse", kernel_type_map, "kernel");
  p_node->add_attr<FloatAttribute>("radius", 0.05f, 0.01f, 0.2f, "radius");
  p_node->add_attr<BoolAttribute>("shrink", false, "expand", "shrink");

  // attribute(s) order
  p_node->set_attr_ordered_key({"kernel", "radius", "shrink"});
}

void compute_expand_shrink_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    // kernel definition
    hmap::Array     kernel_array;
    hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

    kernel_array = hmap::get_kernel(kernel_shape,
                                    (hmap::KernelType)GET("kernel", MapEnumAttribute));

    // core operator
    std::function<void(hmap::Array &, hmap::Array *)> lambda;

    if (GET("shrink", BoolAttribute))
      lambda = [&kernel_array](hmap::Array &x, hmap::Array *p_mask)
      { hmap::shrink(x, kernel_array, p_mask); };
    else
      lambda = [&kernel_array](hmap::Array &x, hmap::Array *p_mask)
      { hmap::expand(x, kernel_array, p_mask); };

    hmap::transform(*p_out, p_mask, lambda);
    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
