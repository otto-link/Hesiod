/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Thermal::Thermal(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Thermal::Thermal");

  // model
  this->node_caption = "Thermal";

  // inputs
  this->input_captions = {"input", "bedrock", "mask"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output", "depo. map"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};

  // attributes
  this->attr["talus_global"] = NEW_ATTR_FLOAT(1.f, 0.f, 4.f, "%.3f");
  this->attr["iterations"] = NEW_ATTR_INT(50, 1, 200);

  this->attr_ordered_key = {"talus_global", "iterations"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->deposition_map = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Thermal is an erosion operator used to simulate the process of "
                      "thermal erosion, which is a type of erosion that occurs due to "
                      "temperature fluctuations causing the breakdown and movement of "
                      "soil and rock materials.";

  this->input_descriptions = {
      "Input heightmap.",
      "Bedrock elevation, erosion process cannot carve the heightmap further down this "
      "point.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Eroded heightmap.", "Deposition map"};

  this->attribute_descriptions["talus_global"] = "Repose slope.";
  this->attribute_descriptions["iterations"] = "Number of iterations.";
}

std::shared_ptr<QtNodes::NodeData> Thermal::outData(QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->out);
  case 1:
  default:
    return std::static_pointer_cast<QtNodes::NodeData>(this->deposition_map);
  }
}

void Thermal::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Thermal::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_bedrock = HSD_GET_POINTER(this->bedrock);
    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);

    hmap::HeightMap *p_out = this->out->get_ref();
    hmap::HeightMap *p_deposition_map = this->deposition_map->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET_ATTR_FLOAT("talus_global") / (float)p_out->shape.x;

    hmap::transform(*p_out,
                    p_bedrock,
                    p_mask,
                    p_deposition_map,
                    [this, &talus](hmap::Array &h_out,
                                   hmap::Array *p_bedrock_array,
                                   hmap::Array *p_mask_array,
                                   hmap::Array *p_deposition_array)
                    {
                      hmap::thermal(h_out,
                                    p_mask_array,
                                    hmap::constant(h_out.shape, talus),
                                    GET_ATTR_INT("iterations"),
                                    p_bedrock_array,
                                    p_deposition_array);
                    });

    p_out->smooth_overlap_buffers();

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
