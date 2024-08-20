/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Saturate::Saturate(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Saturate::Saturate");

  // model
  this->node_caption = "Saturate";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["range"] = NEW_ATTR_RANGE();
  this->attr["k_smoothing"] = NEW_ATTR_FLOAT(0.1f, 0.01, 1.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Saturate limits the amplitude of a signal to a predefined range "
                      "while keeping the initial input range.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Saturated heightmap."};

  this->attribute_descriptions["range"] = "Saturation range: define the minimum and "
                                          "maximum values of the saturation process.";
  this->attribute_descriptions["k_smoothing"] = "Smoothing intensity.";
}

std::shared_ptr<QtNodes::NodeData> Saturate::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Saturate::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Saturate::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    hmap::transform(*p_out,
                    [this, &hmin, &hmax](hmap::Array &x)
                    {
                      hmap::saturate(x,
                                     GET_ATTR_RANGE("range").x,
                                     GET_ATTR_RANGE("range").y,
                                     hmin,
                                     hmax,
                                     GET_ATTR_FLOAT("k_smoothing"));
                    });
    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
