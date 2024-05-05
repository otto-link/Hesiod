/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
// #include <vector>

#include "highmap/heightmap.hpp"

#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SetAlpha::SetAlpha(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SetAlpha::SetAlpha");

  // model
  this->node_caption = "SetAlpha";

  // inputs
  this->input_captions = {"in", "alpha"};
  this->input_types = {HeightMapRGBAData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"out"};
  this->output_types = {HeightMapRGBAData().type()};

  // attributes
  this->attr["alpha"] = NEW_ATTR_FLOAT(1.f, 0.f, 1.f, "%.2f");
  this->attr["clamp_alpha"] = NEW_ATTR_BOOL(true);
  this->attr_ordered_key = {"alpha", "clamp_alpha"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapRGBAData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "SetAlpha adjusts the transparency of a texture based on an input "
                      "alpha value, which can either be a single scalar or an array. "
                      "When provided with a scalar alpha value, the function uniformly "
                      "adjusts the transparency of the entire texture. Alternatively, "
                      "when given an array of alpha values, it enables fine-grained "
                      "control over the transparency of different parts of the texture, "
                      "allowing for varied opacity across the texture's surface.";

  this->input_descriptions = {"Input texture.",
                              "Transparency (expected to be in [0, 1])."};
  this->output_descriptions = {"Texture with new transparency."};

  this->attribute_descriptions["alpha"] = "Transparency as a scalar value (overriden if "
                                          "this alpha input is set).";
  this->attribute_descriptions["clamp_alpha"] = "Clamp to [0, 1] to input alpha map.";
}

std::shared_ptr<QtNodes::NodeData> SetAlpha::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SetAlpha::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex                 port_index)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
    break;
  case 1:
    this->alpha = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void SetAlpha::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMapRGBA *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap     *p_alpha = HSD_GET_POINTER(this->alpha);
    hmap::HeightMapRGBA *p_out = this->out->get_ref();

    *p_out = *p_in;

    if (p_alpha)
    {
      if (GET_ATTR_BOOL("clamp_alpha"))
        hmap::transform(*p_alpha, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      p_out->set_alpha(*p_alpha);
    }
    else
      p_out->set_alpha(GET_ATTR_FLOAT("alpha"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
