/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_kernel_gabor_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Array>(gnode::PortType::OUT,
                                "kernel",
                                p_node->get_config_ref()->shape);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.1f, 0.001f, 0.2f);
  ADD_ATTR(BoolAttribute, "normalize", false);
  ADD_ATTR(FloatAttribute, "kw", 2.f, 0.01f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "angle", 0.f, -180.f, 180.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius", "normalize", "_SEPARATOR_", "kw", "angle"});
}

void compute_kernel_gabor_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Array *p_out = p_node->get_value_ref<hmap::Array>("kernel");

  int ir = std::max(
      1,
      (int)(GET("radius", FloatAttribute) * p_node->get_config_ref()->shape.x));

  // kernel definition
  hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

  *p_out = hmap::gabor(kernel_shape,
                       GET("kw", FloatAttribute),
                       GET("angle", FloatAttribute));

  if (GET("normalize", BoolAttribute))
    *p_out /= p_out->sum();

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
