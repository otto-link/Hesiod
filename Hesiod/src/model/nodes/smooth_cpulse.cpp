/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SmoothCpulse::SmoothCpulse(const ModelConfig &config) : BaseNode(config)
{
  LOG_DEBUG("SmoothCpulse::SmoothCpulse");
  config.log_debug();

  // model
  this->node_caption = "SmoothCpulse";
  this->input_types = {HeightMapData().type(), MaskData().type()};
  this->output_types = {HeightMapData().type()};
  this->input_captions = {"input", "mask"};
  this->output_captions = {"output"};

  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f, "%.3f");

  this->out = std::make_shared<HeightMapData>(config);

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> SmoothCpulse::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SmoothCpulse::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void SmoothCpulse::compute()
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

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    p_mask,
                    [&ir](hmap::Array &x, hmap::Array *p_mask)
                    { hmap::smooth_cpulse(x, ir, p_mask); });
    p_out->smooth_overlap_buffers();
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
