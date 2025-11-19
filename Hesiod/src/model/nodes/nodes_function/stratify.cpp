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

void setup_stratify_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, IntAttribute, "n_strata", 3, 1, 16);
  ADD_ATTR(node, FloatAttribute, "strata_noise", 0.f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "gamma", 0.7f, 0.1f, 2.f);
  ADD_ATTR(node, FloatAttribute, "gamma_noise", 0.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"seed", "n_strata", "strata_noise", "gamma", "gamma_noise"});

  add_deprecated_warning_label(node, "Use Strata node.");
}

void compute_stratify_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

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
                                    GET(node, "n_strata", IntAttribute) + 1,
                                    GET(node, "strata_noise", FloatAttribute),
                                    GET(node, "seed", SeedAttribute));

    float gmin = std::max(0.01f,
                          GET(node, "gamma", FloatAttribute) *
                              (1.f - GET(node, "gamma_noise", FloatAttribute)));
    float gmax = GET(node, "gamma", FloatAttribute) *
                 (1.f + GET(node, "gamma_noise", FloatAttribute));

    auto gs = hmap::random_vector(gmin,
                                  gmax,
                                  GET(node, "n_strata", IntAttribute),
                                  GET(node, "seed", SeedAttribute));

    hmap::transform(
        {p_out, p_mask, p_noise},
        [&node, &hs, &gs](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_noise = p_arrays[2];

          hmap::stratify(*pa_out, pa_mask, hs, gs, pa_noise);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
