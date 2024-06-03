/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

White::White(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("White::White");

  // model
  this->node_caption = "White";

  // inputs
  this->input_captions = {"envelope"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"seed", "_SEPARATOR_", "inverse", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "White noise operator generates a random signal with a flat power "
                      "spectral density.";

  this->input_descriptions = {"Output noise amplitude envelope."};
  this->output_descriptions = {"Generated noise."};

  this->attribute_descriptions["seed"] = "Random seed number.";
}

std::shared_ptr<QtNodes::NodeData> White::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void White::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex /* port_index */)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  this->envelope = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void White::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);
  hmap::HeightMap *p_out = this->out->get_ref();

  int seed = (int)GET_ATTR_SEED("seed");

  hmap::fill(*p_out,
             [&seed](hmap::Vec2<int> shape)
             { return hmap::white(shape, 0.f, 1.f, (uint)seed++); });

  // add envelope
  if (p_env)
  {
    float hmin = p_out->min();
    hmap::transform(*p_out,
                    *p_env,
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

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
