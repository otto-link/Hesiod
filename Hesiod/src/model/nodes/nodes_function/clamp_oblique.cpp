/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_clamp_oblique_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("vmax", "Clamp Value", 0.5f, -FLT_MAX, FLT_MAX);
  node.add_attr<FloatAttribute>("angle", "Angle", 0.f, -180.f, 180.f, "{:.1f}°");
  node.add_attr<FloatAttribute>("slope", "Slope", 0.2f, 0.f, 4.f);
  node.add_attr<Vec2FloatAttribute>("center", "Center");
  node.add_attr<BoolAttribute>("use_max_operator", "Clamp Mode", "Max", "Min", true);
  node.add_attr<FloatAttribute>("k", "Smoothing", 0.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Clamp Parameters",
                             "vmax",
                             "angle",
                             "slope",
                             "center",
                             "use_max_operator",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Smoothing",
                             "k",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_clamp_oblique_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::transform({p_out, p_in},
                    [&node](std::vector<hmap::Array *> p_arrays,
                            hmap::Vec2<int> /* shape */,
                            hmap::Vec4<float> bbox)
                    {
                      hmap::Array *pa_out = p_arrays[0];
                      hmap::Array *pa_in = p_arrays[1];

                      *pa_out = *pa_in;

                      hmap::clamp_oblique_plane(
                          *pa_out,
                          node.get_attr<FloatAttribute>("vmax"),
                          node.get_attr<FloatAttribute>("angle"),
                          node.get_attr<FloatAttribute>("slope"),
                          node.get_attr<BoolAttribute>("use_max_operator"),
                          node.get_attr<FloatAttribute>("k"),
                          node.get_attr<Vec2FloatAttribute>("center"),
                          bbox);
                    });

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
