/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HeightMapToMask::HeightMapToMask(const ModelConfig &config) : BaseNode(config)
{
  LOG_DEBUG("HeightMapToMask::HeightMapToMask");
  config.log_debug();

  // model
  this->node_caption = "HeightMapToMask";
  this->input_types = {HeightMapData().type()};
  this->output_types = {MaskData().type()};
  this->input_captions = {"input"};
  this->output_captions = {"mask"};

  // TODO add controls

  this->mask = std::make_shared<MaskData>(config);

  // GUI
  this->p_preview_data = (QtNodes::NodeData *)this->mask.get();
  this->p_viewer2d_data = this->p_preview_data;

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> HeightMapToMask::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->mask);
}

void HeightMapToMask::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void HeightMapToMask::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_mask = this->mask->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_mask = *p_in;

    // clamp to [0, 1]
    hmap::transform(*p_mask, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
