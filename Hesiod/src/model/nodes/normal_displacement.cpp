/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

NormalDisplacement::NormalDisplacement(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("NormalDisplacement::NormalDisplacement");

  // model
  this->node_caption = "NormalDisplacement";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.2f);
  this->attr["amount"] = NEW_ATTR_FLOAT(5.f, 0.f, 20.f);
  this->attr["reverse"] = NEW_ATTR_BOOL(false);
  this->attr["iterations"] = NEW_ATTR_INT(1, 1, 10);

  this->attr_ordered_key = {"radius", "amount", "reverse", "iterations"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "NormalDisplacement applies a displacement to the terrain along "
                      "the normal direction.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
  this->attribute_descriptions["amount"] = "Displacement scaling.";
  this->attribute_descriptions["reverse"] = "Reverse displacement direction.";
  this->attribute_descriptions
      ["iterations"] = "Number of successive use of the operator.";
}

std::shared_ptr<QtNodes::NodeData> NormalDisplacement::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void NormalDisplacement::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void NormalDisplacement::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(0, (int)(GET_ATTR_FLOAT("radius") * p_in->shape.x));

    for (int it = 0; it < GET_ATTR_INT("iterations"); it++)
    {
      hmap::transform(*p_out,
                      p_mask,
                      [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                      {
                        hmap::normal_displacement(x,
                                                  p_mask,
                                                  GET_ATTR_FLOAT("amount"),
                                                  ir,
                                                  GET_ATTR_BOOL("reverse"));
                      });
      p_out->smooth_overlap_buffers();
    }

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
