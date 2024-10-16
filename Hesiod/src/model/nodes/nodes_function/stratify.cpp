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
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<IntAttribute>("n_strata", 3, 1, 16, "n_strata");
  p_node->add_attr<FloatAttribute>("strata_noise", 0.f, 0.f, 1.f, "strata_noise");
  p_node->add_attr<FloatAttribute>("gamma", 0.7f, 0.01f, 5.f, "gamma");
  p_node->add_attr<FloatAttribute>("gamma_noise", 0.f, 0.f, 1.f, "gamma_noise");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"seed", "n_strata", "strata_noise", "gamma", "gamma_noise"});
}

void compute_stratify_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_noise = p_node->get_value_ref<hmap::HeightMap>("noise");
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

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

    hmap::transform(*p_out,
                    p_mask,
                    p_noise,
                    [p_node, &hs, &gs](hmap::Array &h_out,
                                       hmap::Array *p_mask_array,
                                       hmap::Array *p_noise_array)
                    { hmap::stratify(h_out, p_mask_array, hs, gs, p_noise_array); });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
