/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Noise::Noise(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Noise::Noise");

  // model
  this->node_caption = "Noise";

  // inputs
  this->input_captions = {"dx", "dy", "envelope"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};
  this->out = std::make_shared<HeightMapData>(p_config);

  // attributes
  this->attr["noise_type"] = NEW_ATTR_MAPENUM(noise_type_map);
  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"noise_type",
                            "_SEPARATOR_",
                            "kw",
                            "seed",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> Noise::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Noise::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->envelope = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Noise::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  // base noise function
  hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
  hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);

  hmap::fill(*this->out->get_ref(),
             p_dx,
             p_dy,
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::noise((hmap::NoiseType)GET_ATTR_MAPENUM("noise_type"),
                                  shape,
                                  GET_ATTR_WAVENB("kw"),
                                  GET_ATTR_SEED("seed"),
                                  p_noise_x,
                                  p_noise_y,
                                  nullptr,
                                  bbox);
             });

  // add envelope
  hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);

  if (p_env)
  {
    float hmin = this->out->get_ref()->min();
    hmap::transform(*this->out->get_ref(),
                    *p_env,
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

  // post-process
  post_process_heightmap(*this->out->get_ref(),
                         GET_ATTR_BOOL("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR_BOOL("remap"),
                         GET_ATTR_RANGE("remap_range"));

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
