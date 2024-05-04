/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/lerp.hpp"


namespace hesiod
{

Lerp::Lerp(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Lerp::Lerp");

  // model
  this->node_caption = "Lerp";

  // inputs
  this->input_captions = {"a", "b", "t"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["t"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);

  this->attr_ordered_key = {"t"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Lerp operator, short for linear interpolation, is a method "
                      "for smoothly transitioning between two values over a specified "
                      "range or interval.";

  this->input_descriptions = {"Start heightmap (t = 0).",
                              "End heightmap (t = 1).",
                              "Lerp factor, expected in [0, 1]."};
  this->output_descriptions = {"Interpolated heightmap."};

  this->attribute_descriptions
      ["t"] = "Lerp factor (in [0, 1]). Used only if the node input 't' is not set.";
}

std::shared_ptr<QtNodes::NodeData> Lerp::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Lerp::setInData(std::shared_ptr<QtNodes::NodeData> data,
                     QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->a = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->b = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->t = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Lerp::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_a = HSD_GET_POINTER(this->a);
  hmap::HeightMap *p_b = HSD_GET_POINTER(this->b);
  hmap::HeightMap *p_t = HSD_GET_POINTER(this->t);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_a && p_b)
  {
    if (p_t)
      hmap::transform(*p_out,
                      *p_a,
                      *p_b,
                      *p_t,
                      [](hmap::Array &out, hmap::Array &a, hmap::Array &b, hmap::Array &t)
                      { out = hmap::lerp(a, b, t); });
    else
      hmap::transform(*p_out,
                      *p_a,
                      *p_b,
                      [this](hmap::Array &out, hmap::Array &a, hmap::Array &b)
                      { out = hmap::lerp(a, b, GET_ATTR_FLOAT("t")); });
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
