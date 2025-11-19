/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/operator.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_stratify_oblique_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<IntAttribute>("n_strata", "n_strata", 3, 1, 16);

  node.add_attr<FloatAttribute>("strata_noise", "strata_noise", 0.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("gamma", "gamma", 0.7f, 0.01f, 5.f);

  node.add_attr<FloatAttribute>("gamma_noise", "gamma_noise", 0.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("talus_global", "talus_global", 1.f, 0.f, FLT_MAX);

  node.add_attr<FloatAttribute>("angle", "angle", 30.f, -180.f, 180.f);

  // attribute(s) order
  node.set_attr_ordered_key({"seed",
                             "n_strata",
                             "strata_noise",
                             "gamma",
                             "gamma_noise",
                             "talus_global",
                             "angle"});

  add_deprecated_warning_label(node, "Use Strata node.");
}

void compute_stratify_oblique_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_noise = node.get_value_ref<hmap::Heightmap>("noise");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float zmin = p_out->min();
    float zmax = p_out->max();

    auto hs = hmap::linspace_jitted(zmin,
                                    zmax,
                                    node.get_attr<IntAttribute>("n_strata") + 1,
                                    node.get_attr<FloatAttribute>("strata_noise"),
                                    node.get_attr<SeedAttribute>("seed"));

    float gmin = std::max(0.01f,
                          node.get_attr<FloatAttribute>("gamma") *
                              (1.f - node.get_attr<FloatAttribute>("gamma_noise")));
    float gmax = node.get_attr<FloatAttribute>("gamma") *
                 (1.f + node.get_attr<FloatAttribute>("gamma_noise"));

    auto gs = hmap::random_vector(gmin,
                                  gmax,
                                  node.get_attr<IntAttribute>("n_strata"),
                                  node.get_attr<SeedAttribute>("seed"));

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;

    hmap::transform(*p_out,
                    p_mask,
                    p_noise,
                    [&node, &hs, &gs, &talus](hmap::Array &h_out,
                                              hmap::Array *p_mask_array,
                                              hmap::Array *p_noise_array)
                    {
                      hmap::stratify_oblique(h_out,
                                             p_mask_array,
                                             hs,
                                             gs,
                                             talus,
                                             node.get_attr<FloatAttribute>("angle"),
                                             p_noise_array);
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
