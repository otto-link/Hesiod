/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_mix_normal_map_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "normal map base");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "normal map detail");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "normal map", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("detail_scaling", 1.f, 0.f, 4.f, "detail_scaling");
  p_node->add_attr<MapEnumAttribute>("blending_method",
                                     hmap::normal_map_blending_method_as_string,
                                     "blending_method");

  // attribute(s) order
  p_node->set_attr_ordered_key({});
}

void compute_mix_normal_map_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMapRGBA *p_in1 = p_node->get_value_ref<hmap::HeightMapRGBA>(
      "normal map base");
  hmap::HeightMapRGBA *p_in2 = p_node->get_value_ref<hmap::HeightMapRGBA>(
      "normal map detail");

  if (p_in1 && p_in2)
  {
    hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>("normal map");

    *p_out = hmap::mix_normal_map_rgba(
        *p_in1,
        *p_in2,
        GET("detail_scaling", FloatAttribute),
        (hmap::NormalMapBlendingMethod)GET("blending_method", MapEnumAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod