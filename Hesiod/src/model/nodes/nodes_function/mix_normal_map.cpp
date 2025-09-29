/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mix_normal_map_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map base");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map detail");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "normal map", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "detail_scaling", 1.f, 0.f, 4.f);
  ADD_ATTR(EnumAttribute, "blending_method", hmap::normal_map_blending_method_as_string);

  // attribute(s) order
  p_node->set_attr_ordered_key({"detail_scaling", "blending_method"});
}

void compute_mix_normal_map_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::HeightmapRGBA *p_in1 = p_node->get_value_ref<hmap::HeightmapRGBA>(
      "normal map base");
  hmap::HeightmapRGBA *p_in2 = p_node->get_value_ref<hmap::HeightmapRGBA>(
      "normal map detail");

  if (p_in1 && p_in2)
  {
    hmap::HeightmapRGBA *p_out = p_node->get_value_ref<hmap::HeightmapRGBA>("normal map");

    *p_out = hmap::mix_normal_map_rgba(
        *p_in1,
        *p_in2,
        GET("detail_scaling", FloatAttribute),
        (hmap::NormalMapBlendingMethod)GET("blending_method", EnumAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
