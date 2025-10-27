/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_translate_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(Vec2FloatAttribute, "center");
  ADD_ATTR(BoolAttribute, "periodic", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"center", "periodic"});
}

void compute_translate_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  // base noise function
  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::Array  dx_array, dy_array;
    hmap::Array *p_dx_array = nullptr, *p_dy_array = nullptr;

    if (p_dx)
    {
      dx_array = p_dx->to_array();
      p_dx_array = &dx_array;
    }

    if (p_dy)
    {
      dy_array = p_dy->to_array();
      p_dy_array = &dy_array;
    }

    hmap::Vec2<float> center = GET("center", Vec2FloatAttribute);

    z_array = hmap::translate(z_array,
                              center.x - 0.5f,
                              center.y - 0.5f,
                              GET("periodic", BoolAttribute),
                              p_dx_array,
                              p_dy_array);

    p_out->from_array_interp_nearest(z_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
