/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ExportPath::ExportPath(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ExportPath::ExportPath");

  // model
  this->node_caption = "ExportPath";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {};
  this->output_types = {};

  // attributes
  this->attr["fname"] = NEW_ATTR_FILENAME("path.csv");
  this->attr["auto_export"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"fname", "auto_export"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->compute();
  }

  // documentation
  this->description = "ExportPath is an operator for exporting path data to a csv file.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {};

  this->attribute_descriptions["fname"] = "Export file name.";
  this->attribute_descriptions["auto_export"] =
      "Decides whether the export is automatically performed when the node is updated.";
}

std::shared_ptr<QtNodes::NodeData> ExportPath::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void ExportPath::setInData(std::shared_ptr<QtNodes::NodeData> data,
                           QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void ExportPath::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Path *p_in = HSD_GET_POINTER(this->in);

  if (p_in && GET_ATTR_BOOL("auto_export"))
  {
    Q_EMIT this->computingStarted();

    p_in->to_csv(GET_ATTR_FILENAME("fname"));

    // not output, do not propagate
    Q_EMIT this->computingFinished();
  }
}

} // namespace hesiod
