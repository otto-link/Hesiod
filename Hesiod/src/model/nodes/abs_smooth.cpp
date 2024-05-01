/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

AbsSmooth::AbsSmooth(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("AbsSmooth::AbsSmooth");

  // model
  this->node_caption = "AbsSmooth";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["mu"] = NEW_ATTR_FLOAT(0.05f, 0.001f, 0.4f, "%.3f");
  this->attr["vshift"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key =
      {"mu", "vshift", "_SEPARATOR_", "inverse", "remap", "remap_range"};
  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Apply a smooth absolute value function to every values.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions["mu"] = "Smoothing intensity.";
  this->attribute_descriptions
      ["vshift"] = "Reference value for the zero-equivalent value of the absolute value.";
}

std::shared_ptr<QtNodes::NodeData> AbsSmooth::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void AbsSmooth::setInData(std::shared_ptr<QtNodes::NodeData> data,
                          QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void AbsSmooth::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    hmap::transform(
        *p_out,
        *p_in,
        [this](hmap::Array &out, hmap::Array &in)
        { out = hmap::abs_smooth(in, GET_ATTR_FLOAT("mu"), GET_ATTR_FLOAT("vshift")); });

    // post-process
    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR_BOOL("remap"),
                           GET_ATTR_RANGE("remap_range"));
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
