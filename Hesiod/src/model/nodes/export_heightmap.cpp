/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ExportHeightmap::ExportHeightmap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ExportHeightmap::ExportHeightmap");

  // model
  this->node_caption = "ExportHeightmap";

  // inputs
  this->input_captions = {"texture"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {};
  this->output_types = {};

  // attributes
  this->attr["fname"] = NEW_ATTR_FILENAME("hmap.png");
  this->attr["format"] = NEW_ATTR_MAPENUM(heightmap_export_format_map, "png (8 bit)");
  this->attr["auto_export"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"fname", "format", "auto_export"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> ExportHeightmap::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void ExportHeightmap::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void ExportHeightmap::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in && GET_ATTR_BOOL("auto_export"))
  {
    std::string fname = GET_ATTR_FILENAME("fname");

    switch (GET_ATTR_MAPENUM("format"))
    {
    case export_format::png8bit:
      p_in->to_array().to_png_grayscale_8bit(fname);
      break;

    case export_format::png16bit:
      p_in->to_array().to_png_grayscale_16bit(fname);
      break;

    case export_format::raw16bit:
      p_in->to_array().to_raw_16bit(fname);
      break;
    }
  }

  // not output, do not propagate
  Q_EMIT this->computingFinished();
}

} // namespace hesiod
