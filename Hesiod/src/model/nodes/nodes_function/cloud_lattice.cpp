/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/grids.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_lattice_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  std::vector<float> default_value = {0.1f, 0.1f};

  p_node->add_attr<WaveNbAttribute>("delta", default_value, 0.f, 0.2f, true, "delta");
  p_node->add_attr<WaveNbAttribute>("stagger_ratio",
                                    default_value,
                                    0.f,
                                    1.f,
                                    true,
                                    "stagger_ratio");
  p_node->add_attr<WaveNbAttribute>("jitter_ratio",
                                    default_value,
                                    0.f,
                                    1.f,
                                    true,
                                    "jitter_ratio");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"delta", "stagger_ratio", "jitter_ratio", "seed", "_SEPARATOR_", "remap_range"});
}

void compute_cloud_lattice_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

  std::vector<float> x, y, v;
  hmap::Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f};

  hmap::random_grid(x,
                    y,
                    v,
                    GET("seed", SeedAttribute),
                    GET("delta", WaveNbAttribute),
                    GET("stagger_ratio", WaveNbAttribute),
                    GET("jitter_ratio", WaveNbAttribute),
                    bbox);

  *p_out = hmap::Cloud(x, y, v);

  if (GET_ATTR("remap_range", RangeAttribute, is_active))
    p_out->remap_values(GET("remap_range", RangeAttribute)[0],
                        GET("remap_range", RangeAttribute)[1]);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
