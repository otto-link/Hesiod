/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_kmeans_clustering3_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "feature 1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "feature 2");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "feature 3");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<std::vector<hmap::Heightmap>>(gnode::PortType::OUT, "scoring");

  // attribute(s)
  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<IntAttribute>("nclusters", "nclusters", 6, 1, 16);

  node.add_attr<FloatAttribute>("weights.x", "weights.x", 1.f, 0.01f, 2.f);

  node.add_attr<FloatAttribute>("weights.y", "weights.y", 1.f, 0.01f, 2.f);

  node.add_attr<FloatAttribute>("weights.z", "weights.z", 1.f, 0.01f, 2.f);

  node.add_attr<BoolAttribute>("normalize_inputs", "normalize_inputs", true);

  node.add_attr<BoolAttribute>("compute_scoring", "compute_scoring", true);

  // attribute(s) order
  node.set_attr_ordered_key({"seed",
                             "nclusters",
                             "weights.x",
                             "weights.y",
                             "weights.z",
                             "normalize_inputs",
                             "_SEPARATOR_",
                             "compute_scoring"});
}

void compute_kmeans_clustering3_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("feature 1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("feature 2");
  hmap::Heightmap *p_in3 = node.get_value_ref<hmap::Heightmap>("feature 3");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  std::vector<hmap::Heightmap>
      *p_scoring = node.get_value_ref<std::vector<hmap::Heightmap>>("scoring");

  if (p_in1 && p_in2 && p_in3)
  {

    // work on arrays (not tileable so far)
    hmap::Array              labels;
    std::vector<hmap::Array> scoring_arrays;

    {
      hmap::Array z = p_out->to_array(p_out->shape);
      hmap::Array a1 = p_in1->to_array(p_out->shape);
      hmap::Array a2 = p_in2->to_array(p_out->shape);
      hmap::Array a3 = p_in3->to_array(p_out->shape);

      if (node.get_attr<BoolAttribute>("normalize_inputs"))
      {
        hmap::remap(a1);
        hmap::remap(a2);
        hmap::remap(a3);
      }

      hmap::Vec3<float> weights = {node.get_attr<FloatAttribute>("weights.x"),
                                   node.get_attr<FloatAttribute>("weights.y"),
                                   node.get_attr<FloatAttribute>("weights.z")};

      if (node.get_attr<BoolAttribute>("normalize_inputs"))
        labels = hmap::kmeans_clustering3(a1,
                                          a2,
                                          a3,
                                          node.get_attr<IntAttribute>("nclusters"),
                                          &scoring_arrays,
                                          nullptr,
                                          weights,
                                          node.get_attr<SeedAttribute>("seed"));
      else
        labels = hmap::kmeans_clustering3(a1,
                                          a2,
                                          a3,
                                          node.get_attr<IntAttribute>("nclusters"),
                                          nullptr,
                                          nullptr,
                                          weights,
                                          node.get_attr<SeedAttribute>("seed"));
    }

    p_out->from_array_interp_nearest(labels);

    // retrieve scoring data
    p_scoring->clear();
    p_scoring->reserve(node.get_attr<IntAttribute>("nclusters"));

    for (size_t k = 0; k < scoring_arrays.size(); k++)
    {
      hmap::Heightmap h;
      h.set_sto(CONFIG(node));
      h.from_array_interp_nearest(scoring_arrays[k]);
      p_scoring->push_back(h);
    }
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
