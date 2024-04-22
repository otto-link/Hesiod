/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Gradient::Gradient(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Gradient::Gradient");

  // model
  this->node_caption = "Gradient";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"dx", "dy"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};

  // attributes
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE(hmap::Vec2<float>(0.f, 1.f), 0.f, 1.f);
  this->attr_ordered_key = {"remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->dx = std::make_shared<HeightMapData>(p_config);
    this->dy = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> Gradient::outData(QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->dx);
  case 1:
    return std::static_pointer_cast<QtNodes::NodeData>(this->dy);
  }
}

void Gradient::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Gradient::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_dx = this->dx->get_ref();
  hmap::HeightMap *p_dy = this->dy->get_ref();

  if (p_in)
  {
    hmap::transform(*p_dx,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in) { hmap::gradient_x(in, out); });

    hmap::transform(*p_dy,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in) { hmap::gradient_y(in, out); });

    p_dx->smooth_overlap_buffers();
    p_dy->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_dx,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR_BOOL("remap"),
                           GET_ATTR_RANGE("remap_range"));

    post_process_heightmap(*p_dy,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR_BOOL("remap"),
                           GET_ATTR_RANGE("remap_range"));
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
