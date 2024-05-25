/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CloudToPath::CloudToPath(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("CloudToPath::CloudToPath");

  // model
  this->node_caption = "CloudToPath";

  // inputs
  this->input_captions = {"cloud"};
  this->input_types = {CloudData().type()};

  // outputs
  this->output_captions = {"path"};
  this->output_types = {PathData().type()};

  // attributes
  this->attr["closed"] = NEW_ATTR_BOOL(false);
  this->attr["reorder_nns"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"closed", "reorder_nns"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<PathData>();
    this->compute();
  }

  // documentation
  this->description = "CloudToPath convert a Cloud to a Path.";

  this->input_descriptions = {"Input cloud."};
  this->output_descriptions = {"Output path."};

  this->attribute_descriptions
      ["closed"] = "Decides whether the path is open and closed on itself.";
  this->attribute_descriptions["reorder_nns"] =
      "Decides whether the path points are reordered using a nearest neighbor search.";
}

std::shared_ptr<QtNodes::NodeData> CloudToPath::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CloudToPath::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<CloudData>(data);

  this->compute();
}

// --- computing

void CloudToPath::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::Cloud *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Path *p_out = this->out->get_ref();

    // convert the input
    *p_out = hmap::Path(p_in->points);

    p_out->closed = GET_ATTR_BOOL("closed");

    if (GET_ATTR_BOOL("reorder_nns"))
      p_out->reorder_nns();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
