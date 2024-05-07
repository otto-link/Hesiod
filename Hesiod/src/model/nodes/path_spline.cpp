/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathBspline::PathBspline(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("PathBspline::PathBspline");

  // model
  this->node_caption = "PathBspline";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {PathData().type()};

  // attributes
  this->attr["edge_divisions"] = NEW_ATTR_INT(10, 1, 32);

  this->attr_ordered_key = {"edge_divisions"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<PathData>();
    this->compute();
  }

  // documentation
  this->description = "PathBspline uses Bspline interpolation to replace sharp angles "
                      "and "
                      "straight segments with smooth, flowing curves.";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Output path."};

  this->attribute_descriptions
      ["edge_divisions"] = "Edge sub-divisions of each edge. After the operation, the "
                           "path is remeshed based on this new sub-division.";
}

std::shared_ptr<QtNodes::NodeData> PathBspline::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathBspline::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void PathBspline::compute()
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
      p_out->bspline(GET_ATTR_INT("edge_divisions"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
