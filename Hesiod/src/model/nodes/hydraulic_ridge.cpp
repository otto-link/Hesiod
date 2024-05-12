/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HydraulicRidge::HydraulicRidge(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("HydraulicRidge::HydraulicRidge");

  // model
  this->node_caption = "HydraulicRidge";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["talus_global"] = NEW_ATTR_FLOAT(16.f, 0.01f, 32.f);
  this->attr["intensity"] = NEW_ATTR_FLOAT(0.5f, 0.f, 2.f);
  this->attr["erosion_factor"] = NEW_ATTR_FLOAT(1.5f, 0.f, 4.f);
  this->attr["smoothing_factor"] = NEW_ATTR_FLOAT(0.5f, 0.01f, 2.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.01f, 0.f, 0.1f);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr_ordered_key = {"talus_global",
                            "intensity",
                            "erosion_factor",
                            "smoothing_factor",
                            "noise_ratio",
                            "radius",
                            "seed"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "HydraulicRidge .";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["talus_global"] = "Ridge slope.";
  this->attribute_descriptions["intensity"] = "Intensity mask, expected in [0, 1] "
                                              "(applied as a post-processing).";
  this->attribute_descriptions["erosion_factor"] =
      "Erosion factor, generally in ]0, 10]. Smaller values tend to flatten the map.";
  this->attribute_descriptions
      ["smoothing_factor"] = "Smooothing factor in ]0, 1] (1 for no smoothing).";
  this->attribute_descriptions["noise_ratio"] = "Ridge talus noise ratio in [0, 1].";
  this->attribute_descriptions
      ["radius"] = "Pre-filter radius with respect to the domain size.";
  this->attribute_descriptions["seed"] = "Random seed number.";
}

std::shared_ptr<QtNodes::NodeData> HydraulicRidge::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void HydraulicRidge::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void HydraulicRidge::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    // handle masking
    hmap::Array *p_mask_array = nullptr;
    hmap::Array  mask_array;

    if (p_mask)
    {
      mask_array = p_mask->to_array();
      p_mask_array = &mask_array;
    }

    // backup amplitude for post-process remapping
    float zmin = z_array.min();
    float zmax = z_array.max();

    float talus = GET_ATTR_FLOAT("talus_global") / (float)p_out->shape.x;
    int   ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::hydraulic_ridge(z_array,
                          talus,
                          p_mask_array,
                          GET_ATTR_FLOAT("intensity"),
                          GET_ATTR_FLOAT("erosion_factor"),
                          GET_ATTR_FLOAT("smoothing_factor"),
                          GET_ATTR_FLOAT("noise_ratio"),
                          ir,
                          GET_ATTR_SEED("seed"));

    hmap::remap(z_array, zmin, zmax);

    p_out->from_array_interp_nearest(z_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
