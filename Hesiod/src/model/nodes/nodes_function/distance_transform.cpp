/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_distance_transform_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(EnumAttribute,
           "transform_type",
           distance_transform_type_map,
           "Approx. (fast)");
  ADD_ATTR(BoolAttribute, "reverse_input", false);
  ADD_ATTR(FloatAttribute, "threshold", 0.5f, -1.f, 2.f);
  ADD_ATTR(BoolAttribute, "reverse_output", false);
  ADD_ATTR(BoolAttribute, "remap", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"transform_type",
                                "reverse_input",
                                "reverse_output",
                                "threshold",
                                "_SEPARATOR_",
                                "remap"});
}

void compute_distance_transform_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        {p_out, p_in},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = *pa_in;
          make_binary(*pa_out, GET("threshold", FloatAttribute));

          if (GET("reverse_input", BoolAttribute))
            *pa_out = 1.f - *pa_out;

          auto type = static_cast<hmap::DistanceTransformType>(
              GET("transform_type", EnumAttribute));

          switch (type)
          {
          case hmap::DistanceTransformType::DT_EXACT:
            *pa_out = hmap::distance_transform(*pa_out);
            break;
          case hmap::DistanceTransformType::DT_MANHATTAN:
            *pa_out = hmap::distance_transform_manhattan(*pa_out);
            break;
          case hmap::DistanceTransformType::DT_APPROX:
          default:
            *pa_out = hmap::distance_transform_approx(*pa_out);
            break;
          }
        },
        hmap::TransformMode::SINGLE_ARRAY); // mandatory

    // post-process
    post_process_heightmap(p_node,
                           *p_out,
                           GET("reverse_output", BoolAttribute),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET("remap", BoolAttribute),
                           {0.f, 1.f});
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
