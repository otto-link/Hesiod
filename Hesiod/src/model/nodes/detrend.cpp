/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/operator.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Detrend::Detrend(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Detrend::Detrend");

  // model
  this->node_caption = "Detrend";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  // none

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Detrend operator is used to adjust the elevation using a very "
                      "basic (uncorrect) detrending operator.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Detrended heightmap."};
}

std::shared_ptr<QtNodes::NodeData> Detrend::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Detrend::setInData(std::shared_ptr<QtNodes::NodeData> data,
                        QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Detrend::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    float min = p_in->min();
    float max = p_in->max();

    // work on a single array
    hmap::Array z_array = p_in->to_array();
    z_array = hmap::detrend_reg(z_array);
    p_out->from_array_interp(z_array);

    if (min != max)
      p_out->remap(min, max);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
