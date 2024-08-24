/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ExportNormalMap::ExportNormalMap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ExportNormalMap::ExportNormalMap");

  // model
  this->node_caption = "ExportNormalMap";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {};
  this->output_types = {};

  // attributes
  this->attr["fname"] = NEW_ATTR_FILENAME("nmap.png");
  this->attr["16bit"] = NEW_ATTR_BOOL(false);
  this->attr["auto_export"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"fname", "16bit", "auto_export"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->compute();
  }

  // documentation
  this->description = "ExportNormalMap is an operator for exporting the normal map of an "
                      "heightmap as a PNG image file.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {};

  this->attribute_descriptions["fname"] = "Export file name.";
  this->attribute_descriptions
      ["16bit"] = "Whether the output file is a 16bit PNG file (instead of 8bit).";
  this->attribute_descriptions["auto_export"] =
      "Decides whether the export is automatically performed when the node is updated.";
}

std::shared_ptr<QtNodes::NodeData> ExportNormalMap::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void ExportNormalMap::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void ExportNormalMap::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in && GET_ATTR_BOOL("auto_export"))
  {
    Q_EMIT this->computingStarted();

    std::string fname = GET_ATTR_FILENAME("fname");

    if (GET_ATTR_BOOL("16bit"))
      hmap::export_normal_map_png(fname, p_in->to_array(), CV_16U);
    else
      hmap::export_normal_map_png(fname, p_in->to_array(), CV_8U);

    // not output, do not propagate
    Q_EMIT this->computingFinished();
  }
}

} // namespace hesiod
