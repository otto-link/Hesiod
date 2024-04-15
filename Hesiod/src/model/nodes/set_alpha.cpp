/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
// #include <vector>

#include "highmap/heightmap.hpp"

#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SetAlpha::SetAlpha(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SetAlpha::SetAlpha");

  // model
  this->node_caption = "SetAlpha";

  // inputs
  this->input_captions = {"in", "alpha"};
  this->input_types = {HeightMapRGBAData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"out"};
  this->output_types = {HeightMapRGBAData().type()};
  this->out = std::make_shared<HeightMapRGBAData>(p_config);

  // attributes
  this->attr["alpha"] = NEW_ATTR_FLOAT(1.f, 0.f, 1.f, "%.2f");
  this->attr_ordered_key = {"alpha"};

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> SetAlpha::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SetAlpha::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex                 port_index)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
    break;
  case 1:
    this->alpha = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void SetAlpha::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMapRGBA *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap     *p_alpha = HSD_GET_POINTER(this->alpha);
  hmap::HeightMapRGBA *p_out = this->out->get_ref();

  if (p_in)
  {
    *p_out = *p_in;

    if (p_alpha)
      p_out->set_alpha(*p_alpha);
    else
      p_out->set_alpha(GET_ATTR_FLOAT("alpha"));
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod