/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/laplace.hpp"


namespace hesiod
{

Laplace::Laplace(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Laplace::Laplace");

  // model
  this->node_caption = "Laplace";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.25f);
  this->attr["iterations"] = NEW_ATTR_INT(3, 1, 10);

  this->attr_ordered_key = {"sigma", "iterations"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Laplace smoothing filter reduces noise and smooth out variations "
                      "in pixel intensity while preserving the overall structure of an "
                      "image.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["sigma"] = "Filtering strength.";
  this->attribute_descriptions
      ["iterations"] = "Number of successive filtering iterations.";
}

std::shared_ptr<QtNodes::NodeData> Laplace::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Laplace::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void Laplace::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(
        *p_out,
        p_mask,
        [this](hmap::Array &x, hmap::Array *p_mask) {
          hmap::laplace(x, p_mask, GET_ATTR_FLOAT("sigma"), GET_ATTR_INT("iterations"));
        });
    p_out->smooth_overlap_buffers();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
