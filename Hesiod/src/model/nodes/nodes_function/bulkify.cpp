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

void setup_bulkify_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("amplitude", "amplitude", 1.f, -1.f, 4.f);

  node.add_attr<EnumAttribute>("bulk_type",
                               "bulk_type",
                               enum_mappings.primitive_type_map);

  node.add_attr<Vec2FloatAttribute>("center", "center");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "_TEXT_Bulk Shape",
                             "amplitude",
                             "bulk_type",
                             "_TEXT_Position",
                             "center",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node, true);
}

void compute_bulkify_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");

    hmap::transform(
        {p_out, p_in, p_dx, p_dy},
        [&node](std::vector<hmap::Array *> p_arrays,
                hmap::Vec2<int> /* shape */,
                hmap::Vec4<float> bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_dx = p_arrays[2];
          hmap::Array *pa_dy = p_arrays[3];

          *pa_out = hmap::bulkify(
              *pa_in,
              (hmap::PrimitiveType)node.get_attr<EnumAttribute>("bulk_type"),
              node.get_attr<FloatAttribute>("amplitude"),
              pa_dx,
              pa_dy,
              node.get_attr<Vec2FloatAttribute>("center"),
              bbox);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->remap(p_in->min(), p_in->max());
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
