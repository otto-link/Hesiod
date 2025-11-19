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

void setup_stratify_multiscale_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, SeedAttribute, "seed");

  std::vector<int>   n_strata = {4, 4};
  std::vector<float> strata_noise = {0.5f, 0.5f};
  std::vector<float> gamma_list = {0.6f, 0.5f};
  std::vector<float> gamma_noise = {0.2f, 0.2f};

  ADD_ATTR(node, VecIntAttribute, "n_strata", n_strata, 1, 32);
  ADD_ATTR(node, VecFloatAttribute, "strata_noise", strata_noise, 0.f, 1.f);
  ADD_ATTR(node, VecFloatAttribute, "gamma_list", gamma_list, 0.01f, 5.f);
  ADD_ATTR(node, VecFloatAttribute, "gamma_noise", gamma_noise, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"seed", "n_strata", "strata_noise", "gamma_list", "gamma_noise"});

  add_deprecated_warning_label(node, "Use Strata node.");
}

void compute_stratify_multiscale_node(BaseNode &node)
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

    // make sure the input vectors have the same size and adjust their
    // content if needed (attribute widgets do not communicate, use a
    // workaround to avoid issues with possibly different vector
    // sizes). Everything is driven by the "n_strata" vector

    // TODO find a way to add an "update_widget" method to the AbstractAttribute

    std::vector<int>   n_strata = GET(node, "n_strata", VecIntAttribute);
    std::vector<float> strata_noise(n_strata.size());
    std::vector<float> gamma_list(n_strata.size());
    std::vector<float> gamma_noise(n_strata.size());

    for (size_t k = 0; k < n_strata.size(); k++)
    {
      if (k >= GET(node, "strata_noise", VecFloatAttribute).size())
        strata_noise[k] = 0.f;
      else
        strata_noise[k] = GET(node, "strata_noise", VecFloatAttribute)[k];

      if (k >= GET(node, "gamma_list", VecFloatAttribute).size())
        gamma_list[k] = 2.f;
      else
        gamma_list[k] = GET(node, "gamma_list", VecFloatAttribute)[k];

      if (k >= GET(node, "gamma_noise", VecFloatAttribute).size())
        gamma_noise[k] = 0.f;
      else
        gamma_noise[k] = GET(node, "gamma_noise", VecFloatAttribute)[k];
    }

    // eventually compute
    hmap::transform(*p_out,
                    p_mask,
                    p_noise,
                    [&node, zmin, zmax, n_strata, strata_noise, gamma_list, gamma_noise](
                        hmap::Array &h_out,
                        hmap::Array *p_mask_array,
                        hmap::Array *p_noise_array)
                    {
                      hmap::stratify_multiscale(h_out,
                                                zmin,
                                                zmax,
                                                n_strata,
                                                strata_noise,
                                                gamma_list,
                                                gamma_noise,
                                                GET(node, "seed", SeedAttribute),
                                                p_mask_array,
                                                p_noise_array);
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
