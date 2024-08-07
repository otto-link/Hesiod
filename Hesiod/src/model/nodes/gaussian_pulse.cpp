/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

GaussianPulse::GaussianPulse(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("GaussianPulse::GaussianPulse");

  // model
  this->node_caption = "GaussianPulse";

  // inputs
  this->input_captions = {"dx", "dy", "control"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"radius",
                            "center.x",
                            "center.y",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "GaussianPulse generates a Gaussian pulse.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction).",
      "Control parameter, acts as a multiplier for the weight parameter."};
  this->output_descriptions = {"Gaussian heightmap."};

  this->attribute_descriptions["radius"] = "Pulse half-width.";
  this->attribute_descriptions["center.x"] = "Center x coordinate.";
  this->attribute_descriptions["center.y"] = "Center y coordinate.";
}

std::shared_ptr<QtNodes::NodeData> GaussianPulse::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void GaussianPulse::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->dx = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->dy = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->ctrl = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void GaussianPulse::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
  hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);
  hmap::HeightMap *p_ctrl = HSD_GET_POINTER(this->ctrl);
  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             p_ctrl,
             [this, &center](hmap::Vec2<int>   shape,
                             hmap::Vec4<float> bbox,
                             hmap::Array      *p_noise_x,
                             hmap::Array      *p_noise_y,
                             hmap::Array      *p_ctrl)
             {
               return hmap::gaussian_pulse(shape,
                                           GET_ATTR_FLOAT("radius"),
                                           p_ctrl,
                                           p_noise_x,
                                           p_noise_y,
                                           nullptr,
                                           center,
                                           bbox);
             });

  // post-process
  post_process_heightmap(*p_out,
                         GET_ATTR_BOOL("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR_BOOL("remap"),
                         GET_ATTR_RANGE("remap_range"));

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
