/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SharpenCone::SharpenCone(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SharpenCone::SharpenCone");

  // model
  this->node_caption = "SharpenCone";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f);
  this->attr["scale"] = NEW_ATTR_FLOAT(0.1f, 0.f, 2.f);

  this->attr_ordered_key = {"radius", "scale"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "SharpenCone is a cone kernel-based sharpen operator enhancing "
                      "sharpness by emphasizing edges and fine details using a radial "
                      "gradient shape.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
  this->attribute_descriptions["scale"] = "Sharpening intensity.";
}

std::shared_ptr<QtNodes::NodeData> SharpenCone::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SharpenCone::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void SharpenCone::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    p_mask,
                    [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                    { hmap::sharpen_cone(x, p_mask, ir, GET_ATTR_FLOAT("scale")); });

    p_out->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
