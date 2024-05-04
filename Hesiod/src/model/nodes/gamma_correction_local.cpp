/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/gamma_correction_local.hpp"


namespace hesiod
{

GammaCorrectionLocal::GammaCorrectionLocal(const ModelConfig *p_config)
    : BaseNode(p_config)
{
  LOG_DEBUG("GammaCorrectionLocal::GammaCorrectionLocal");

  // model
  this->node_caption = "GammaCorrectionLocal";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.2f);
  this->attr["gamma"] = NEW_ATTR_FLOAT(2.f, 0.01f, 10.f);
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f);

  this->attr_ordered_key = {"radius", "gamma", "k"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Gamma correction involves applying a nonlinear transformation to "
                      "the pixel values of the heightmap. For GammaCorrectionLocal, the "
                      "transformation parameters are locally defined within a perimeter "
                      "'radius'.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
  this->attribute_descriptions["gamma"] = "Gamma exponent.";
  this->attribute_descriptions["k"] = "Smoothing factor (typically in [0, 1]).";
}

std::shared_ptr<QtNodes::NodeData> GammaCorrectionLocal::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void GammaCorrectionLocal::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                     QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void GammaCorrectionLocal::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    float hmin = p_out->min();
    float hmax = p_out->max();
    p_out->remap(0.f, 1.f, hmin, hmax);
    hmap::transform(*p_out,
                    p_mask,
                    [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                    {
                      hmap::gamma_correction_local(x,
                                                   GET_ATTR_FLOAT("gamma"),
                                                   ir,
                                                   p_mask,
                                                   GET_ATTR_FLOAT("k"));
                    });
    p_out->remap(hmin, hmax, 0.f, 1.f);
    p_out->smooth_overlap_buffers();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
