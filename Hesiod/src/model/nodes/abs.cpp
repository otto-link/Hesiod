/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Abs::Abs(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Abs::Abs");

  // model
  this->node_caption = "Abs";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["vshift"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Apply an absolute function to every values.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions
      ["vshift"] = "Reference value for the zero-equivalent value of the absolute value.";
}

std::shared_ptr<QtNodes::NodeData> Abs::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Abs::setInData(std::shared_ptr<QtNodes::NodeData> data,
                    QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Abs::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    hmap::transform(*p_out,
                    *p_in,
                    [this](hmap::Array &out, hmap::Array &in)
                    { out = hmap::abs(in - GET_ATTR_FLOAT("vshift")); });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
