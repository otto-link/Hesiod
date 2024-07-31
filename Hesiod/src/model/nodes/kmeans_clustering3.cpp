/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

KmeansClustering3::KmeansClustering3(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("KmeansClustering3::KmeansClustering3");

  // model
  this->node_caption = "KmeansClustering3";

  // inputs
  this->input_captions = {"feature 1", "feature 2", "feature 3"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output", "scoring"};
  this->output_types = {HeightMapData().type(), HeightMapVectorData().type()};

  // attributes
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["nclusters"] = NEW_ATTR_INT(6, 1, 16);
  this->attr["weights.x"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["weights.y"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["weights.z"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["normalize_inputs"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key =
      {"seed", "nclusters", "weights.x", "weights.y", "weights.z", "normalize_inputs"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->scoring = std::make_shared<HeightMapVectorData>();
    this->compute();
  }

  // documentation
  this->description = "KmeansClustering2 node groups the data into clusters based on the "
                      "values of the three input features.";

  this->input_descriptions = {
      "First measurable property or characteristic of the data points being analyzed "
      "(e.g elevation, gradient norm, etc...",
      "First measurable property or characteristic of the data points being analyzed "
      "(e.g elevation, gradient norm, etc...",
      "Third measurable property or characteristic of the data points being analyzed "
      "(e.g elevation, gradient norm, etc..."};
  this->output_descriptions = {"Cluster labelling."};

  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["nclusters"] = "Number of clusters.";
  this->attribute_descriptions["weights.x"] = "Weight of the first feature.";
  this->attribute_descriptions["weights.y"] = "Weight of the second feature.";
  this->attribute_descriptions["weights.y"] = "Weight of the third feature.";
  this->attribute_descriptions["normalize_inputs"] =
      "Determine whether the feature amplitudes are normalized before the clustering.";
}

std::shared_ptr<QtNodes::NodeData> KmeansClustering3::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void KmeansClustering3::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                  QtNodes::PortIndex                 port_index)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  switch (port_index)
  {
  case 0:
    this->in1 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->in2 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->in3 = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void KmeansClustering3::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap              *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMap              *p_in2 = HSD_GET_POINTER(this->in2);
  hmap::HeightMap              *p_in3 = HSD_GET_POINTER(this->in3);
  hmap::HeightMap              *p_out = this->out->get_ref();
  std::vector<hmap::HeightMap> *p_scoring = this->scoring->get_ref();

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

      if (GET_ATTR_BOOL("normalize_inputs"))
      {
        hmap::remap(a1);
        hmap::remap(a2);
        hmap::remap(a3);
      }

      hmap::Vec3<float> weights = {GET_ATTR_FLOAT("weights.x"),
                                   GET_ATTR_FLOAT("weights.y"),
                                   GET_ATTR_FLOAT("weights.z")};

      labels = hmap::kmeans_clustering3(a1,
                                        a2,
                                        a3,
                                        GET_ATTR_INT("nclusters"),
                                        nullptr, // &scoring_arrays,
                                        weights,
                                        GET_ATTR_SEED("seed"));
    }

    p_out->from_array_interp_nearest(labels);

    // // retrieve scoring data
    // p_scoring->clear();
    // p_scoring->reserve(GET_ATTR_INT("nclusters"));

    // for (size_t k = 0; k < scoring_arrays.size(); k++)
    // {
    //   hmap::HeightMap h;
    //   h.set_sto(p_config->shape, p_config->tiling, p_config->overlap);
    //   h.from_array_interp_nearest(scoring_arrays[k]);
    //   p_scoring->push_back(h);
    // }
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
