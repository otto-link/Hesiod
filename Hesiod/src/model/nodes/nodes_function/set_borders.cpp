/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_set_borders_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius", 0.4f, 0.f, 0.5f);
  ADD_ATTR(BoolAttribute, "unique_border_value", false);
  ADD_ATTR(FloatAttribute, "value_west", -0.5f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "value_east", 0.5f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "value_north", 0.5f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "value_south", -0.5f, -FLT_MAX, FLT_MAX);

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius",
                                "unique_border_value",
                                "_SEPARATOR_",
                                "value_west",
                                "value_east",
                                "value_north",
                                "value_south"});
}

void compute_set_borders_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_in->shape.x));
    hmap::Vec4<int> buffer_sizes(ir, ir, ir, ir);

    hmap::Vec4<float> border_values;

    if (GET("unique_border_value", BoolAttribute))
      border_values = {GET("value_west", FloatAttribute),
                       GET("value_west", FloatAttribute),
                       GET("value_west", FloatAttribute),
                       GET("value_west", FloatAttribute)};
    else
      border_values = {GET("value_west", FloatAttribute),
                       GET("value_east", FloatAttribute),
                       GET("value_north", FloatAttribute),
                       GET("value_south", FloatAttribute)};

    hmap::transform(
        {p_out, p_in},
        [border_values, buffer_sizes](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = *pa_in;

          hmap::set_borders(*pa_out, border_values, buffer_sizes);
        },
        hmap::TransformMode::SINGLE_ARRAY);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
