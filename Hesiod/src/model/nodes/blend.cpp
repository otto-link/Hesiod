/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Blend::Blend(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Blend::Blend");

  // model
  this->node_caption = "Blend";

  // inputs
  this->input_captions = {"input 1", "input 2"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["blending_method"] = NEW_ATTR_MAPENUM(blending_method_map, "minimum_smooth");
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"blending_method",
                            "k",
                            "radius",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Blend operator takes two input heightmaps.";

  this->input_descriptions = {"Input heightmap.", "Input heightmap."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions["blending_method"] = "Blending method.";
  this->attribute_descriptions["k"] = "Smoothing intensity (if any).";
  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size (if any).";
}

std::shared_ptr<QtNodes::NodeData> Blend::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Blend::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in1 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->in2 = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Blend::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMap *p_in2 = HSD_GET_POINTER(this->in2);

  if (p_in1 && p_in2)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

    float k = GET_ATTR_FLOAT("k");
    int   ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));
    int   method = GET_ATTR_MAPENUM("blending_method");

    switch (method)
    {
    case BlendingMethod::ADD:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 + a2; };
      break;

    case BlendingMethod::EXCLUSION_BLEND:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_exclusion(a1, a2); };
      break;

    case BlendingMethod::GRADIENTS:
      lambda = [&ir](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_gradients(a1, a2, ir); };
      break;

    case BlendingMethod::MAXIMUM:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::maximum(a1, a2); };
      break;

    case BlendingMethod::MAXIMUM_SMOOTH:
      lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::maximum_smooth(a1, a2, k); };
      break;

    case BlendingMethod::MINIMUM:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::minimum(a1, a2); };
      break;

    case BlendingMethod::MINIMUM_SMOOTH:
      lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::minimum_smooth(a1, a2, k); };
      break;

    case BlendingMethod::MULTIPLY:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 * a2; };
      break;

    case BlendingMethod::MULTIPLY_ADD:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 + a1 * a2; };
      break;

    case BlendingMethod::NEGATE:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_negate(a1, a2); };
      break;

    case BlendingMethod::OVERLAY:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_overlay(a1, a2); };
      break;

    case BlendingMethod::SOFT:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::blend_soft(a1, a2); };
      break;

    case BlendingMethod::SUBSTRACT:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 - a2; };
      break;
    }

    hmap::transform(*p_out, *p_in1, *p_in2, lambda);

    if (method == BlendingMethod::GRADIENTS)
      p_out->smooth_overlap_buffers();

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

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
