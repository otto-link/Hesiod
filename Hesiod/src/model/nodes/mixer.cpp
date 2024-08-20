/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Mixer::Mixer(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Mixer::Mixer");

  // model
  this->node_caption = "Mixer";

  // inputs
  this->input_captions = {"input 1", "input 2", "input 3", "input 4", "t"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Mixer operator takes several input heightmaps and blend them "
                      "according to a mixing parameter expected in [0, 1].";

  this->input_descriptions = {"Input heightmap.",
                              "Input heightmap.",
                              "Input heightmap.",
                              "Input heightmap.",
                              "Mixing parameter, expected in [0, 1]."};
  this->output_descriptions = {"Output heightmap."};
}

std::shared_ptr<QtNodes::NodeData> Mixer::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Mixer::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in1 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->in2 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->in3 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 3:
    this->in4 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 4:
    this->t = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Mixer::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMap *p_in2 = HSD_GET_POINTER(this->in2);
  hmap::HeightMap *p_in3 = HSD_GET_POINTER(this->in3);
  hmap::HeightMap *p_in4 = HSD_GET_POINTER(this->in4);
  hmap::HeightMap *p_t = HSD_GET_POINTER(this->t);

  std::vector<hmap::HeightMap *> ptr_list = {};
  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size() && p_t)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    hmap::transform(*p_out,
                    p_in1,
                    p_in2,
                    p_in3,
                    p_in4,
                    p_t,
                    [](hmap::Array &out,
                       hmap::Array *p_a_in1,
                       hmap::Array *p_a_in2,
                       hmap::Array *p_a_in3,
                       hmap::Array *p_a_in4,
                       hmap::Array *p_a_t)
                    {
                      std::vector<hmap::Array *> arrays = {};

                      for (auto &ptr : {p_a_in1, p_a_in2, p_a_in3, p_a_in4})
                        if (ptr)
                          arrays.push_back(ptr);

                      out = hmap::mixer(*p_a_t, arrays);
                    });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
