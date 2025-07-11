/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_kernel_prim_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Array>(gnode::PortType::OUT,
                                "kernel",
                                p_node->get_config_ref()->shape);

  // attribute(s)
  ADD_ATTR(EnumAttribute, "kernel", kernel_type_map, "cubic_pulse");
  ADD_ATTR(FloatAttribute, "radius", 0.1f, 0.001f, 0.2f);
  ADD_ATTR(BoolAttribute, "normalize", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"kernel", "radius", "normalize"});
}

void compute_kernel_prim_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Array *p_out = p_node->get_value_ref<hmap::Array>("kernel");

  int ir = std::max(
      1,
      (int)(GET("radius", FloatAttribute) * p_node->get_config_ref()->shape.x));

  // kernel definition
  hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

  *p_out = hmap::get_kernel(kernel_shape, (hmap::KernelType)GET("kernel", EnumAttribute));

  if (GET("normalize", BoolAttribute))
    *p_out /= p_out->sum();

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
