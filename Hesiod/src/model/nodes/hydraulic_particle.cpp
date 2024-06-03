/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HydraulicParticle::HydraulicParticle(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("HydraulicParticle::HydraulicParticle");

  // model
  this->node_caption = "HydraulicParticle";

  // inputs
  this->input_captions = {"input", "bedrock", "moisture", "mask"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output", "erosion map", "depo. map"};
  this->output_types = {HeightMapData().type(),
                        HeightMapData().type(),
                        HeightMapData().type()};

  // attributes
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["nparticles"] = NEW_ATTR_INT(40000, 1, 1000000);
  this->attr["c_capacity"] = NEW_ATTR_FLOAT(40.f, 0.1f, 100.f);
  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.1f);
  this->attr["c_deposition"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 0.1f);
  this->attr["drag_rate"] = NEW_ATTR_FLOAT(0.01f, 0.f, 1.f);
  this->attr["evap_rate"] = NEW_ATTR_FLOAT(0.001f, 0.f, 1.f);
  this->attr["c_radius"] = NEW_ATTR_INT(0, 0, 16);

  this->attr_ordered_key = {"seed",
                            "nparticles",
                            "c_capacity",
                            "c_erosion",
                            "c_deposition",
                            "drag_rate",
                            "evap_rate",
                            "c_radius"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->erosion_map = std::make_shared<HeightMapData>(p_config);
    this->deposition_map = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "HydraulicParticle is a particle-based hydraulic erosion operator "
                      "that simulates the erosion and sediment transport processes that "
                      "occur due to the flow of water over a terrain represented by the "
                      "input heightmap. This type of operator models erosion by tracking "
                      "the movement of virtual particles (or sediment particles) as they "
                      "are transported by water flow and interact with the terrain.";

  this->input_descriptions = {
      "Input heightmap.",
      "Bedrock elevation, erosion process cannot carve the heightmap further down this "
      "point.",
      "Moisture map, influences the amount of water locally deposited.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Eroded heightmap.", "Erosion map", "Deposition map"};

  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["nparticles"] = "Number of simulated particles.";
  this->attribute_descriptions["c_capacity"] = "Particle capacity.";
  this->attribute_descriptions["c_erosion"] = "Particle erosion cofficient.";
  this->attribute_descriptions["c_deposition"] = "Particle deposition coefficient.";
  this->attribute_descriptions["drag_rate"] = "Particle drag rate.";
  this->attribute_descriptions["evap_rate"] = "Particle water evaporation rate.";
  this->attribute_descriptions["c_radius"] =
      "Particle radius (c_radius = 0 for a pixel-based algorithm and c_radius > 0 for a "
      "kernel-based algorithm, which can be significantly slower).";
}

std::shared_ptr<QtNodes::NodeData> HydraulicParticle::outData(
    QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->out);
  case 1:
    return std::static_pointer_cast<QtNodes::NodeData>(this->erosion_map);
  case 2:
  default:
    return std::static_pointer_cast<QtNodes::NodeData>(this->deposition_map);
  }
}

void HydraulicParticle::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->bedrock = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->moisture_map = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 3:
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void HydraulicParticle::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_bedrock = HSD_GET_POINTER(this->bedrock);
    hmap::HeightMap *p_moisture_map = HSD_GET_POINTER(this->moisture_map);
    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);

    hmap::HeightMap *p_out = this->out->get_ref();
    hmap::HeightMap *p_erosion_map = this->erosion_map->get_ref();
    hmap::HeightMap *p_deposition_map = this->deposition_map->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    int nparticles_tile = (int)(GET_ATTR_INT("nparticles") / (float)p_out->get_ntiles());

    hmap::transform(*p_out,
                    p_bedrock,
                    p_moisture_map,
                    p_mask,
                    p_erosion_map,
                    p_deposition_map,
                    [this, &nparticles_tile](hmap::Array &h_out,
                                             hmap::Array *p_bedrock_array,
                                             hmap::Array *p_moisture_map_array,
                                             hmap::Array *p_mask_array,
                                             hmap::Array *p_erosion_map_array,
                                             hmap::Array *p_deposition_map_array)
                    {
                      hmap::hydraulic_particle(h_out,
                                               p_mask_array,
                                               nparticles_tile,
                                               GET_ATTR_SEED("seed"),
                                               p_bedrock_array,
                                               p_moisture_map_array,
                                               p_erosion_map_array,
                                               p_deposition_map_array,
                                               GET_ATTR_INT("c_radius"),
                                               GET_ATTR_FLOAT("c_capacity"),
                                               GET_ATTR_FLOAT("c_erosion"),
                                               GET_ATTR_FLOAT("c_deposition"),
                                               GET_ATTR_FLOAT("drag_rate"),
                                               GET_ATTR_FLOAT("evap_rate"));
                    });

    p_out->smooth_overlap_buffers();

    if (p_erosion_map)
    {
      p_erosion_map->smooth_overlap_buffers();
      p_erosion_map->remap();
    }

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
