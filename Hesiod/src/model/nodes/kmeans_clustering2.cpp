/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

KmeansClustering2::KmeansClustering2(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("KmeansClustering2::KmeansClustering2");

  // model
  this->node_caption = "KmeansClustering2";

  // inputs
  this->input_captions = {"feature 1", "feature 2"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["nclusters"] = NEW_ATTR_INT(4, 1, 16);
  this->attr["weights.x"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["weights.y"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["normalize_inputs"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"seed",
                            "nclusters",
                            "weights.x",
                            "weights.y",
                            "normalize_inputs"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "KmeansClustering2 node groups the data into clusters based on the "
                      "values of the two input features.";

  this->input_descriptions = {
      "First measurable property or characteristic of the data points being analyzed "
      "(e.g elevation, gradient norm, etc...",
      "Second measurable property or characteristic of the data points being analyzed "
      "(e.g elevation, gradient norm, etc..."};
  this->output_descriptions = {"Cluster labelling."};

  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["nclusters"] = "Number of clusters.";
  this->attribute_descriptions["weights.x"] = "Weight of the first feature.";
  this->attribute_descriptions["weights.y"] = "Weight of the second feature.";
  this->attribute_descriptions["normalize_inputs"] =
      "Determine whether the feature amplitudes are normalized before the clustering.";
}

std::shared_ptr<QtNodes::NodeData> KmeansClustering2::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void KmeansClustering2::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
  }

  this->compute();
}

// --- computing

void KmeansClustering2::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMap *p_in2 = HSD_GET_POINTER(this->in2);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in1 && p_in2)
  {

    // work on arrays (not tileable so far)
    hmap::Array labels;
    {
      hmap::Array z = p_out->to_array(p_out->shape);
      hmap::Array a1 = p_in1->to_array(p_out->shape);
      hmap::Array a2 = p_in2->to_array(p_out->shape);

      if (GET_ATTR_BOOL("normalize_inputs"))
      {
        hmap::remap(a1);
        hmap::remap(a2);
      }

      hmap::Vec2<float> weights = {GET_ATTR_FLOAT("weights.x"),
                                   GET_ATTR_FLOAT("weights.y")};

      labels = hmap::kmeans_clustering2(a1,
                                        a2,
                                        GET_ATTR_INT("nclusters"),
                                        nullptr,
                                        weights,
                                        GET_ATTR_SEED("seed"));
    }

    p_out->from_array_interp_nearest(labels);
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
