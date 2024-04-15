/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SmoothFill::SmoothFill(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SmoothFill::SmoothFill");

  // model
  this->node_caption = "SmoothFill";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), MaskData().type()};

  // outputs
  this->output_captions = {"output", "depo. map"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};
  this->out = std::make_shared<HeightMapData>(p_config);
  this->deposition_map = std::make_shared<HeightMapData>(p_config);

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.001f, 0.2f, "%.3f");
  this->attr["k"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 1.f);
  this->attr["normalized_map"] = NEW_ATTR_BOOL(true);
  this->attr_ordered_key = {"radius", "k", "normalized_map"};

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> SmoothFill::outData(QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->out);
  case 1:
    return std::static_pointer_cast<QtNodes::NodeData>(this->deposition_map);
  }
}

void SmoothFill::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void SmoothFill::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();
  hmap::HeightMap *p_deposition_map = this->deposition_map->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(
        *p_out,
        p_mask,
        p_deposition_map,
        [this, &ir](hmap::Array &x, hmap::Array *p_mask, hmap::Array *p_deposition)
        { hmap::smooth_fill(x, ir, p_mask, GET_ATTR_FLOAT("k"), p_deposition); });

    p_out->smooth_overlap_buffers();
    p_deposition_map->smooth_overlap_buffers();

    if (GET_ATTR_BOOL("normalized_map"))
      p_deposition_map->remap();
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod