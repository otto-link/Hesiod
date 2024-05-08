/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Path::Path(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Path::Path");

  // model
  this->node_caption = "Path";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"path"};
  this->output_types = {PathData().type()};

  // attributes
  this->attr["path"] = NEW_ATTR_PATH();
  this->attr["closed"] = NEW_ATTR_BOOL(false);
  this->attr_ordered_key = {"path", "closed"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<PathData>();
    this->compute();
  }

  // documentation
  this->description = "Polyline definition.";

  this->input_descriptions = {};
  this->output_descriptions = {"Set of directed points (x, y) and elevations z."};

  this->attribute_descriptions["path"] = "Path data.";
  this->attribute_descriptions
      ["closed"] = "Decides whether the path is open and closed on itself.";
}

std::shared_ptr<QtNodes::NodeData> Path::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Path::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                     QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void Path::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::Path *p_out = this->out->get_ref();

  *p_out = GET_ATTR_PATH("path");
  p_out->closed = GET_ATTR_BOOL("closed");

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod