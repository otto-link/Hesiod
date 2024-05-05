/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HydraulicStream::HydraulicStream(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("HydraulicStream::HydraulicStream");

  // model
  this->node_caption = "HydraulicStream";

  // inputs
  this->input_captions = {"input", "bedrock", "moisture", "mask"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output", "erosion map"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};

  // attributes
  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.1f);
  this->attr["talus_ref"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 10.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.05f, "%.3f");
  this->attr["clipping_ratio"] = NEW_ATTR_FLOAT(10.f, 0.1f, 100.f);

  this->attr_ordered_key = {"c_erosion", "talus_ref", "radius", "clipping_ratio"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->erosion_map = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "HydraulicStream is an hydraulic erosion operator using the flow "
                      "stream approach and an infinite flow direction algorithm for "
                      "simulating the erosion processes. Simulate water flow across the "
                      "terrain using the infinite flow direction algorithm. As water "
                      "flows over the terrain, flow accumulation representing the volume "
                      "of water that passes through each point on the map, is computed "
                      "to evaluate the erosive power of the water flow.";

  this->input_descriptions = {
      "Input heightmap.",
      "Bedrock elevation, erosion process cannot carve the heightmap further down this "
      "point.",
      "Moisture map, influences the amount of water locally deposited.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Eroded heightmap.", "Erosion map"};

  this->attribute_descriptions["c_erosion"] = "Erosion intensity.";
  this->attribute_descriptions["talus_ref"] =
      "Reference talus, with small values of talus_ref  leading to thinner flow streams.";
  this->attribute_descriptions["radius"] = "Carving kernel radius.";
  this->attribute_descriptions["clipping_ratio"] = "Flow accumulation clipping ratio.";
}

std::shared_ptr<QtNodes::NodeData> HydraulicStream::outData(QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->out);
  case 1:
  default:
    return std::static_pointer_cast<QtNodes::NodeData>(this->erosion_map);
  }
}

void HydraulicStream::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void HydraulicStream::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_bedrock = HSD_GET_POINTER(this->bedrock);
    hmap::HeightMap *p_moisture_map = HSD_GET_POINTER(this->moisture_map);
    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);

    hmap::HeightMap *p_out = this->out->get_ref();
    hmap::HeightMap *p_erosion_map = this->erosion_map->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    int ir = (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x);

    hmap::transform(*p_out,
                    p_bedrock,
                    p_moisture_map,
                    p_mask,
                    p_erosion_map,
                    nullptr, // p_deposition_map,
                    [this, &ir](hmap::Array &h_out,
                                hmap::Array *p_bedrock_array,
                                hmap::Array *p_moisture_map_array,
                                hmap::Array *p_mask_array,
                                hmap::Array *p_erosion_map_array,
                                hmap::Array * /*p_deposition_map_array*/)
                    {
                      hmap::hydraulic_stream(h_out,
                                             p_mask_array,
                                             GET_ATTR_FLOAT("c_erosion"),
                                             GET_ATTR_FLOAT("talus_ref"),
                                             p_bedrock_array,
                                             p_moisture_map_array,
                                             p_erosion_map_array,
                                             ir,
                                             GET_ATTR_FLOAT("clipping_ratio"));
                    });

    p_out->smooth_overlap_buffers();

    if (p_erosion_map)
      p_erosion_map->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
