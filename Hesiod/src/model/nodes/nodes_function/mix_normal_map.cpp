/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/constants/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_mix_normal_map_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map base");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map detail");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "normal map", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("detail_scaling", "detail_scaling", 1.f, 0.f, 4.f);

  node.add_attr<EnumAttribute>("blending_method",
                               "blending_method",
                               hmap::normal_map_blending_method_as_string);

  // attribute(s) order
  node.set_attr_ordered_key({"detail_scaling", "blending_method"});
}

void compute_mix_normal_map_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_in1 = node.get_value_ref<hmap::HeightmapRGBA>("normal map base");
  hmap::HeightmapRGBA *p_in2 = node.get_value_ref<hmap::HeightmapRGBA>(
      "normal map detail");

  if (p_in1 && p_in2)
  {
    hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("normal map");

    *p_out = hmap::mix_normal_map_rgba(
        *p_in1,
        *p_in2,
        node.get_attr<FloatAttribute>("detail_scaling"),
        (hmap::NormalMapBlendingMethod)node.get_attr<EnumAttribute>("blending_method"));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
