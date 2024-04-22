/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ColorizeSolid::ColorizeSolid(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ColorizeSolid::ColorizeSolid");

  // model
  this->node_caption = "ColorizeSolid";

  // inputs
  this->input_captions = {};
  this->input_types = {};

  // outputs
  this->output_captions = {"texture"};
  this->output_types = {HeightMapRGBAData().type()};

  // attributes
  this->attr["color"] = NEW_ATTR_COLOR();
  this->attr["alpha"] = NEW_ATTR_FLOAT(1.f, 0.f, 1.f, "%.2f");
  this->attr_ordered_key = {"color", "alpha"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapRGBAData>(p_config);
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> ColorizeSolid::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ColorizeSolid::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                              QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void ColorizeSolid::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  std::vector<float> col3 = GET_ATTR_COLOR("color");

  for (int k = 0; k < 4; k++)
  {
    float color = k < 3 ? col3[k] : GET_ATTR_FLOAT("alpha");
    this->out->get_ref()->rgba[k] = hmap::HeightMap(this->p_config->shape,
                                                    this->p_config->tiling,
                                                    this->p_config->overlap,
                                                    color);
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
