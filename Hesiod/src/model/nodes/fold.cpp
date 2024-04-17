/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Fold::Fold(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Fold::Fold");

  // model
  this->node_caption = "Fold";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.2f);
  this->attr["iterations"] = NEW_ATTR_INT(3, 1, 10);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> Fold::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Fold::setInData(std::shared_ptr<QtNodes::NodeData> data,
                     QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Fold::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    hmap::transform(
        *p_out,
        [this, &hmin, &hmax](hmap::Array &x)
        { hmap::fold(x, hmin, hmax, GET_ATTR_INT("iterations"), GET_ATTR_FLOAT("k")); });

    p_out->remap(hmin, hmax);
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
