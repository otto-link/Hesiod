/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ExportTexture::ExportTexture(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ExportTexture::ExportTexture");

  // model
  this->node_caption = "ExportTexture";

  // inputs
  this->input_captions = {"texture"};
  this->input_types = {HeightMapRGBAData().type()};

  // outputs
  this->output_captions = {};
  this->output_types = {};

  // attributes
  this->attr["fname"] = NEW_ATTR_FILENAME("texture.png");
  this->attr["16 bit"] = NEW_ATTR_BOOL(false);
  this->attr["auto_export"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"fname", "16 bit", "auto_export"};

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> ExportTexture::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void ExportTexture::setInData(std::shared_ptr<QtNodes::NodeData> data,
                              QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapRGBAData>(data);

  this->compute();
}

// --- computing

void ExportTexture::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMapRGBA *p_in = HSD_GET_POINTER(this->in);

  if (p_in && GET_ATTR_BOOL("auto_export"))
  {
    if (GET_ATTR_BOOL("16 bit"))
      p_in->to_png_16bit(GET_ATTR_FILENAME("fname"));
    else
      p_in->to_png_8bit(GET_ATTR_FILENAME("fname"));
  }

  // not output, do not propagate
  Q_EMIT this->computingFinished();
}

} // namespace hesiod
