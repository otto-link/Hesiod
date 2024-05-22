/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathMeanderize::PathMeanderize(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("PathMeanderize::PathMeanderize");

  // model
  this->node_caption = "PathMeanderize";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {PathData().type()};

  // attributes
  this->attr["ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["iterations"] = NEW_ATTR_INT(2, 1, 8);
  this->attr["edge_divisions"] = NEW_ATTR_INT(10, 1, 32);

  this->attr_ordered_key = {"ratio",
                            "noise_ratio",
                            "seed",
                            "iterations",
                            "edge_divisions"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<PathData>();
    this->compute();
  }

  // documentation
  this->description = "PathMeanderize uses Bezier interpolation to add menaders to the "
                      "input path.";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Output path."};

  this->attribute_descriptions["ratio"] = "Meander amplitude ratio.";
  this->attribute_descriptions["noise_ratio"] = "Randomness ratio.";
  this->attribute_descriptions["seed"] = "Random seed number";
  this->attribute_descriptions["iterations"] = "Number of meandering iterations.";
  this->attribute_descriptions
      ["edge_divisions"] = "Edge sub-divisions of each edge. After the operation, the "
                           "path is remeshed based on this new sub-division.";
}

std::shared_ptr<QtNodes::NodeData> PathMeanderize::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathMeanderize::setInData(std::shared_ptr<QtNodes::NodeData> data,
                               QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void PathMeanderize::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::Path *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Path *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->meanderize(GET_ATTR_FLOAT("ratio"),
                        GET_ATTR_FLOAT("noise_ratio"),
                        GET_ATTR_SEED("seed"),
                        GET_ATTR_INT("iterations"),
                        GET_ATTR_INT("edge_divisions"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
