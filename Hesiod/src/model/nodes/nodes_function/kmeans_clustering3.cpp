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

void setup_kmeans_clustering3_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "feature 1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "feature 2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "feature 3");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<std::vector<hmap::Heightmap>>(gnode::PortType::OUT, "scoring");

  // attribute(s)
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(IntAttribute, "nclusters", 6, 1, 16);
  ADD_ATTR(FloatAttribute, "weights.x", 1.f, 0.01f, 2.f);
  ADD_ATTR(FloatAttribute, "weights.y", 1.f, 0.01f, 2.f);
  ADD_ATTR(FloatAttribute, "weights.z", 1.f, 0.01f, 2.f);
  ADD_ATTR(BoolAttribute, "normalize_inputs", true);
  ADD_ATTR(BoolAttribute, "compute_scoring", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed",
                                "nclusters",
                                "weights.x",
                                "weights.y",
                                "weights.z",
                                "normalize_inputs",
                                "_SEPARATOR_",
                                "compute_scoring"});
}

void compute_kmeans_clustering3_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  // base noise function
  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("feature 1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("feature 2");
  hmap::Heightmap *p_in3 = p_node->get_value_ref<hmap::Heightmap>("feature 3");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  std::vector<hmap::Heightmap>
      *p_scoring = p_node->get_value_ref<std::vector<hmap::Heightmap>>("scoring");

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

      if (GET("normalize_inputs", BoolAttribute))
      {
        hmap::remap(a1);
        hmap::remap(a2);
        hmap::remap(a3);
      }

      hmap::Vec3<float> weights = {GET("weights.x", FloatAttribute),
                                   GET("weights.y", FloatAttribute),
                                   GET("weights.z", FloatAttribute)};

      if (GET("normalize_inputs", BoolAttribute))
        labels = hmap::kmeans_clustering3(a1,
                                          a2,
                                          a3,
                                          GET("nclusters", IntAttribute),
                                          &scoring_arrays,
                                          nullptr,
                                          weights,
                                          GET("seed", SeedAttribute));
      else
        labels = hmap::kmeans_clustering3(a1,
                                          a2,
                                          a3,
                                          GET("nclusters", IntAttribute),
                                          nullptr,
                                          nullptr,
                                          weights,
                                          GET("seed", SeedAttribute));
    }

    p_out->from_array_interp_nearest(labels);

    // retrieve scoring data
    p_scoring->clear();
    p_scoring->reserve(GET("nclusters", IntAttribute));

    for (size_t k = 0; k < scoring_arrays.size(); k++)
    {
      hmap::Heightmap h;
      h.set_sto(CONFIG);
      h.from_array_interp_nearest(scoring_arrays[k]);
      p_scoring->push_back(h);
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
