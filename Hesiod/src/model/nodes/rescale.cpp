/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/rescale.hpp"


namespace hesiod
{

Rescale::Rescale(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Rescale::Rescale");

  // model
  this->node_caption = "Rescale";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["scaling"] = NEW_ATTR_FLOAT(1.f, 0.f, 4.f, "%.2f");
  this->attr["centered"] = NEW_ATTR_BOOL(false);
  this->attr_ordered_key = {"scaling", "centered"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Rescale operator involves adjusting each data point by "
                      "multiplying it with a predetermined constant.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Rescaled heightmap."};

  this->attribute_descriptions
      ["scaling"] = "Constant by which each elevation values will be multiplied.";
  this->attribute_descriptions["centered"] = "Determine whether a mean offset is applied "
                                             "to the values to center the scaling.";
}

std::shared_ptr<QtNodes::NodeData> Rescale::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Rescale::setInData(std::shared_ptr<QtNodes::NodeData> data,
                        QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Rescale::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    float vref = 0.f;

    if (GET_ATTR_BOOL("centered"))
      vref = p_out->mean();

    hmap::transform(*p_out,
                    [this, &vref](hmap::Array &x)
                    { hmap::rescale(x, GET_ATTR_FLOAT("scaling"), vref); });
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
