/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/operator.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_stratify_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(IntAttribute, "n_strata", 3, 1, 16);
  ADD_ATTR(FloatAttribute, "strata_noise", 0.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "gamma", 0.7f, 0.1f, 2.f);
  ADD_ATTR(FloatAttribute, "gamma_noise", 0.f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"seed", "n_strata", "strata_noise", "gamma", "gamma_noise"});
}

void compute_stratify_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_noise = p_node->get_value_ref<hmap::Heightmap>("noise");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float zmin = p_out->min();
    float zmax = p_out->max();

    auto hs = hmap::linspace_jitted(zmin,
                                    zmax,
                                    GET("n_strata", IntAttribute) + 1,
                                    GET("strata_noise", FloatAttribute),
                                    GET("seed", SeedAttribute));

    float gmin = std::max(0.01f,
                          GET("gamma", FloatAttribute) *
                              (1.f - GET("gamma_noise", FloatAttribute)));
    float gmax = GET("gamma", FloatAttribute) *
                 (1.f + GET("gamma_noise", FloatAttribute));

    auto gs = hmap::random_vector(gmin,
                                  gmax,
                                  GET("n_strata", IntAttribute),
                                  GET("seed", SeedAttribute));

    hmap::transform(
        {p_out, p_mask, p_noise},
        [p_node, &hs, &gs](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_noise = p_arrays[2];

          hmap::stratify(*pa_out, pa_mask, hs, gs, pa_noise);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
