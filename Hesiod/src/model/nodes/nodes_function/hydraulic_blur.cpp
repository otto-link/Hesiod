/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_blur_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.1f, 0.01f, 0.5f);

  node.add_attr<FloatAttribute>("vmax", "vmax", 0.5f, -1.f, 2.f);

  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 0.1f, 0.f, 1.f);

  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "vmax", "k_smoothing", "_SEPARATOR_", "remap"});
}

void compute_hydraulic_blur_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [&node](hmap::Array &out)
                    {
                      hmap::hydraulic_blur(out,
                                           node.get_attr<FloatAttribute>("radius"),
                                           node.get_attr<FloatAttribute>("vmax"),
                                           node.get_attr<FloatAttribute>("k_smoothing"));
                    });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                           node.get_attr<RangeAttribute>("remap"));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
