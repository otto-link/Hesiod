/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathFractalize::PathFractalize(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("PathFractalize::PathFractalize");

  // model
  this->node_caption = "PathFractalize";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {PathData().type()};

  // attributes
  this->attr["iterations"] = NEW_ATTR_INT(4, 1, 10);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.3f, 0.f, 1.f);
  this->attr["orientation"] = NEW_ATTR_INT(0, 0, 1);
  this->attr["persistence"] = NEW_ATTR_FLOAT(1.f, 0.01f, 4.f);

  this->attr_ordered_key = {"iterations", "seed", "sigma", "orientation", "persistence"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<PathData>();
    this->compute();
  }

  // documentation
  this->description = "PathFractalize fractalizes a polyline using a mid-point "
                      "displacement algorithm. The procedure involves iteratively "
                      "modifying the polyline's geometry to increase its complexity, "
                      "mimicking fractal characteristics.";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Output path."};

  this->attribute_descriptions
      ["iterations"] = "Number of mid-point displacement iterations.";
  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions
      ["sigma"] = "Half-width of the random Gaussian displacement, normalized by the "
                  "distance between points.";
  this->attribute_descriptions["orientation"] =
      "Displacement orientation (0 for random inward/outward displacement, 1 to inflate "
      "the path and -1 to deflate the path).";
  this->attribute_descriptions
      ["persistence"] = "Noise persistence (with iteration number).";
}

std::shared_ptr<QtNodes::NodeData> PathFractalize::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathFractalize::setInData(std::shared_ptr<QtNodes::NodeData> data,
                               QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void PathFractalize::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Path *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Path *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->fractalize(GET_ATTR_INT("iterations"),
                        GET_ATTR_SEED("seed"),
                        GET_ATTR_FLOAT("sigma"),
                        GET_ATTR_INT("orientation"),
                        GET_ATTR_FLOAT("persistence"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
