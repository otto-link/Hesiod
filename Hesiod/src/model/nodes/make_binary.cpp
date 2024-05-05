/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

MakeBinary::MakeBinary(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("MakeBinary::MakeBinary");

  // model
  this->node_caption = "MakeBinary";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["threshold"] = NEW_ATTR_FLOAT(0.f, -1.f, 1.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "MakeBinary is a thresholding operator. It transforms an input "
                      "heightmap into a binary heightmap, where each pixel is assigned "
                      "either a value of 0 or 1, depending on whether its intensity "
                      "value surpasses a specified threshold.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Binary heightmap."};

  this->attribute_descriptions["threshold"] = "Threshold value.";
}

std::shared_ptr<QtNodes::NodeData> MakeBinary::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void MakeBinary::setInData(std::shared_ptr<QtNodes::NodeData> data,
                           QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void MakeBinary::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [this](hmap::Array &x)
                    { hmap::make_binary(x, GET_ATTR_FLOAT("threshold")); });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
