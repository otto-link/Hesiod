/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HeightMapToMask::HeightMapToMask(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("HeightMapToMask::HeightMapToMask");

  // model
  this->node_caption = "HeightMapToMask";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"mask"};
  this->output_types = {MaskData().type()};
  this->mask = std::make_shared<MaskData>(p_config);

  // attributes
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");
  this->attr["saturate"] = NEW_ATTR_BOOL(false);
  this->attr["saturate_range"] = NEW_ATTR_RANGE();
  this->attr["saturate_k"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 1.f, "%.2f");

  this->attr_ordered_key = {"inverse",
                            "smoothing",
                            "smoothing_radius",
                            "saturate",
                            "saturate_range",
                            "saturate_k"};

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
  Q_EMIT this->computingStarted();

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

  // post-process
  int ir = std::max(
      1,
      (int)(GET_ATTR_FLOAT("smoothing_radius") * this->mask->get_ref()->shape.x));

  post_process_heightmap(*this->mask->get_ref(),
                         GET_ATTR_BOOL("inverse"),
                         GET_ATTR_BOOL("smoothing"),
                         ir,
                         GET_ATTR_BOOL("saturate"),
                         GET_ATTR_RANGE("saturate_range"),
                         GET_ATTR_FLOAT("saturate_k"),
                         false, // remap
                         {0.f, 0.f});

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
