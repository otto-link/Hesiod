/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

NoiseParberry::NoiseParberry(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("NoiseParberry::NoiseParberry");

  // model
  this->node_caption = "NoiseParberry";

  // inputs
  this->input_captions = {"dx", "dy", "envelope"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["octaves"] = NEW_ATTR_INT(8, 0, 32);
  this->attr["weight"] = NEW_ATTR_FLOAT(0.7f, 0.f, 1.f);
  this->attr["persistence"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["lacunarity"] = NEW_ATTR_FLOAT(2.f, 0.01f, 4.f);
  this->attr["mu"] = NEW_ATTR_FLOAT(1.02f, 1.f, 1.2f, "%.3f");

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"kw",
                            "seed",
                            "octaves",
                            "weight",
                            "persistence",
                            "lacunarity",
                            "_SEPARATOR_",
                            "mu",
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
  this->description = "NoiseParberry, variant of NoiseFbm, is a Perlin noise based "
                      "terrain generator from Ian Parberry, Tobler's First Law of "
                      "Geography, Self Similarity, and Perlin Noise: A Large Scale "
                      "Analysis of Gradient Distribution in Southern Utah with "
                      "Application to Procedural Terrain Generation.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction).",
      "Output noise amplitude envelope."};
  this->output_descriptions = {"Generated noise."};

  this->attribute_descriptions["kw"] = "Noise wavenumbers (kx, ky) for each directions.";
  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["octaves"] = "Number of octaves.";
  this->attribute_descriptions["weight"] = "Octave weighting.";
  this->attribute_descriptions["persistence"] = "Octave persistence.";
  this->attribute_descriptions["lacunarity"] = "Wavenumber ratio between each octaves.";
  this->attribute_descriptions["mu"] = "Gradient magnitude exponent.";
}

std::shared_ptr<QtNodes::NodeData> NoiseParberry::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void NoiseParberry::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void NoiseParberry::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
  hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);
  hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);
  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::noise_parberry(shape,
                                           GET_ATTR_WAVENB("kw"),
                                           GET_ATTR_SEED("seed"),
                                           GET_ATTR_INT("octaves"),
                                           GET_ATTR_FLOAT("weight"),
                                           GET_ATTR_FLOAT("persistence"),
                                           GET_ATTR_FLOAT("lacunarity"),
                                           GET_ATTR_FLOAT("mu"),
                                           nullptr,
                                           p_noise_x,
                                           p_noise_y,
                                           nullptr,
                                           bbox);
             });

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