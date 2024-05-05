/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SmoothFill::SmoothFill(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SmoothFill::SmoothFill");

  // model
  this->node_caption = "SmoothFill";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output", "depo. map"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.001f, 0.2f, "%.3f");
  this->attr["k"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 1.f);
  this->attr["normalized_map"] = NEW_ATTR_BOOL(true);
  this->attr_ordered_key = {"radius", "k", "normalized_map"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->deposition_map = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "SmoothFill is a smoothing technique that takes the maximum "
                      "between the input and the filtered field to simulate a sediment "
                      "deposition effect.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap.", "Deposition map."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
  this->attribute_descriptions["k"] = "Smoothing intensity of the maximum operator.";
  this->attribute_descriptions
      ["normalized_map"] = "Decides if the deposition map is normalized.";
}

std::shared_ptr<QtNodes::NodeData> SmoothFill::outData(QtNodes::PortIndex port_index)
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

void SmoothFill::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void SmoothFill::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();
    hmap::HeightMap *p_deposition_map = this->deposition_map->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(
        *p_out,
        p_mask,
        p_deposition_map,
        [this, &ir](hmap::Array &x, hmap::Array *p_mask, hmap::Array *p_deposition)
        { hmap::smooth_fill(x, ir, p_mask, GET_ATTR_FLOAT("k"), p_deposition); });

    p_out->smooth_overlap_buffers();
    p_deposition_map->smooth_overlap_buffers();

    if (GET_ATTR_BOOL("normalized_map"))
      p_deposition_map->remap();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
