/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

RecurveKura::RecurveKura(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("RecurveKura::RecurveKura");

  // model
  this->node_caption = "RecurveKura";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes

  this->attr["a"] = NEW_ATTR_FLOAT(2.f, 0.01f, 4.f);
  this->attr["b"] = NEW_ATTR_FLOAT(2.f, 0.01f, 4.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "RecurveKura is an operator based on the Kumaraswamy distribution "
                      "that can be used to adjust the amplitude levels of a dataset.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["a"] = "Primarily controls the rightward skewness and "
                                      "tail behavior of the distribution.";
  this->attribute_descriptions
      ["b"] = "Primarily controls the leftward skewness and tail behavior.";
}

std::shared_ptr<QtNodes::NodeData> RecurveKura::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void RecurveKura::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void RecurveKura::compute()
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

    float hmin = p_out->min();
    float hmax = p_out->max();

    p_out->remap(0.f, 1.f, hmin, hmax);

    hmap::transform(
        *p_out,
        p_mask,
        [this](hmap::Array &x, hmap::Array *p_mask)
        { hmap::recurve_kura(x, GET_ATTR_FLOAT("a"), GET_ATTR_FLOAT("b"), p_mask); });

    p_out->remap(hmin, hmax, 0.f, 1.f);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
