/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Rugosity::Rugosity(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Rugosity::Rugosity");

  // model
  this->node_caption = "Rugosity";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f);
  this->attr["clamp_max"] = NEW_ATTR_BOOL(false);
  this->attr["vc_max"] = NEW_ATTR_FLOAT(1.f, 0.f, 10.f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");

  this->attr_ordered_key = {"radius",
                            "clamp_max",
                            "vc_max",
                            "_SEPARATOR_",
                            "inverse",
                            "smoothing",
                            "smoothing_radius"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Rugosity identifies heightmap rugosity, i.e. the roughness or "
                      "irregularity of the surface.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Valley width heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
  this->attribute_descriptions
      ["clamp_max"] = "Decides whether the rugosity values are clamped.";
  this->attribute_descriptions["vc_max"] = "Rugosity clamping upper bound.";
}

std::shared_ptr<QtNodes::NodeData> Rugosity::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Rugosity::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Rugosity::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    *p_in,
                    [&ir](hmap::Array &out, hmap::Array &in)
                    { out = hmap::rugosity(in, ir); });

    if (GET_ATTR_BOOL("clamp_max"))
      hmap::transform(*p_out,
                      [this](hmap::Array &x)
                      { hmap::clamp_max(x, GET_ATTR_FLOAT("vc_max")); });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           GET_ATTR_BOOL("smoothing"),
                           GET_ATTR_FLOAT("smoothing_radius"),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           true, // remap
                           {0.f, 1.f});

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
