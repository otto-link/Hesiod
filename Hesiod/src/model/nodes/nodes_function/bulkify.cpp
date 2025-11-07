/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_bulkify_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "amplitude", 1.f, -1.f, 4.f);
  ADD_ATTR(EnumAttribute, "bulk_type", enum_mappings.primitive_type_map);
  ADD_ATTR(Vec2FloatAttribute, "center");

  // attribute(s) order
  p_node->set_attr_ordered_key({"_GROUPBOX_BEGIN_Main parameters",
                                "_TEXT_Bulk Shape",
                                "amplitude",
                                "bulk_type",
                                "_TEXT_Position",
                                "center",
                                "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(p_node, true);
}

void compute_bulkify_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");

    hmap::transform(
        {p_out, p_in, p_dx, p_dy},
        [p_node](std::vector<hmap::Array *> p_arrays,
                 hmap::Vec2<int> /* shape */,
                 hmap::Vec4<float> bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_dx = p_arrays[2];
          hmap::Array *pa_dy = p_arrays[3];

          *pa_out = hmap::bulkify(*pa_in,
                                  (hmap::PrimitiveType)GET("bulk_type", EnumAttribute),
                                  GET("amplitude", FloatAttribute),
                                  pa_dx,
                                  pa_dy,
                                  GET("center", Vec2FloatAttribute),
                                  bbox);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_out->remap(p_in->min(), p_in->max());
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
