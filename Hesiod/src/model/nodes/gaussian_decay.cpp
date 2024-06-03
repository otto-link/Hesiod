/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

GaussianDecay::GaussianDecay(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("GaussianDecay::GaussianDecay");

  // model
  this->node_caption = "GaussianDecay";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.1f, 0.001f, 0.2f, "%.3f");

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Apply a Gaussian function to every values.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions["sigma"] =
      "Gaussian half-width, represents the distance from the peak of the Gaussian curve "
      "to the points where the amplitude drops to half of its maximum value.";
}

std::shared_ptr<QtNodes::NodeData> GaussianDecay::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void GaussianDecay::setInData(std::shared_ptr<QtNodes::NodeData> data,
                              QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void GaussianDecay::compute()
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
                    { out = hmap::gaussian_decay(in, GET_ATTR_FLOAT("sigma")); });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
