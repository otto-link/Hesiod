/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

GammaCorrection::GammaCorrection(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("GammaCorrection::GammaCorrection");

  // model
  this->node_caption = "GammaCorrection";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), MaskData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["gamma"] = NEW_ATTR_FLOAT(2.f, 0.01f, 10.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> GammaCorrection::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void GammaCorrection::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->mask = std::dynamic_pointer_cast<MaskData>(data);
  }

  this->compute();
}

// --- computing

void GammaCorrection::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();
    p_out->remap(0.f, 1.f, hmin, hmax);
    hmap::transform(*p_out,
                    p_mask,
                    [this](hmap::Array &x, hmap::Array *p_mask)
                    { hmap::gamma_correction(x, GET_ATTR_FLOAT("gamma"), p_mask); });
    p_out->remap(hmin, hmax, 0.f, 1.f);
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
