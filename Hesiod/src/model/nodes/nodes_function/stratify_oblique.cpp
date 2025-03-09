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

void setup_stratify_oblique_node(BaseNode *p_node)
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
  ADD_ATTR(FloatAttribute, "gamma", 0.7f, 0.01f, 5.f);
  ADD_ATTR(FloatAttribute, "gamma_noise", 0.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "talus_global", 1.f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "angle", 30.f, -180.f, 180.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed",
                                "n_strata",
                                "strata_noise",
                                "gamma",
                                "gamma_noise",
                                "talus_global",
                                "angle"});
}

void compute_stratify_oblique_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

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

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::transform(*p_out,
                    p_mask,
                    p_noise,
                    [p_node, &hs, &gs, &talus](hmap::Array &h_out,
                                               hmap::Array *p_mask_array,
                                               hmap::Array *p_noise_array)
                    {
                      hmap::stratify_oblique(h_out,
                                             p_mask_array,
                                             hs,
                                             gs,
                                             talus,
                                             GET("angle", FloatAttribute),
                                             p_noise_array);
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
