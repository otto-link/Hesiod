/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

WhiteDensityMap::WhiteDensityMap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("WhiteDensityMap::WhiteDensityMap");

  // model
  this->node_caption = "WhiteDensityMap";

  // inputs
  this->input_captions = {"density", "envelope"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

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
  this->description = "WhiteDensityMap noise operator generates a random signal with a "
                      "flat power and a spatial density defined by an input heightmap.";

  this->input_descriptions = {"Noise spatial density, expected in [0, 1]."};
  this->input_descriptions = {"Output noise amplitude envelope."};
  this->output_descriptions = {"Generated noise."};

  this->attribute_descriptions["seed"] = "Random seed number.";
}

std::shared_ptr<QtNodes::NodeData> WhiteDensityMap::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void WhiteDensityMap::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->density = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->envelope = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void WhiteDensityMap::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_density = HSD_GET_POINTER(this->density);

  if (p_density)
  {
    hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);
    hmap::HeightMap *p_out = this->out->get_ref();

    // base noise function
    int seed = GET_ATTR_SEED("seed");

    hmap::transform(*p_out,
                    *p_density,
                    [this, &seed](hmap::Array &h_out, hmap::Array &density_map)
                    { h_out = hmap::white_density_map(density_map, (uint)seed++); });

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
}

} // namespace hesiod
