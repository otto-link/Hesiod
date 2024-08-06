/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HydraulicBlur::HydraulicBlur(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("HydraulicBlur::HydraulicBlur");

  // model
  this->node_caption = "HydraulicBlur";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 0.5f);
  this->attr["vmax"] = NEW_ATTR_FLOAT(0.5f, -1.f, 2.f);
  this->attr["k_smoothing"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);

  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key =
      {"radius", "vmax", "k_smoothing", "_SEPARATOR_", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Hydraulic Blur applied a cell-based hydraulic erosion using a "
                      "nonlinear diffusion model.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Eroded heightmap."};

  this->attribute_descriptions
      ["radius"] = "Gaussian filter radius (with respect to a unit domain).";
  this->attribute_descriptions["vmax"] = "Maximum elevation for the details.";
  this->attribute_descriptions["k_smoothing"] = "Smoothing factor, if any.";
}

std::shared_ptr<QtNodes::NodeData> HydraulicBlur::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void HydraulicBlur::setInData(std::shared_ptr<QtNodes::NodeData> data,
                              QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void HydraulicBlur::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [this](hmap::Array &out)
                    {
                      hmap::hydraulic_blur(out,
                                           GET_ATTR_FLOAT("radius"),
                                           GET_ATTR_FLOAT("vmax"),
                                           GET_ATTR_FLOAT("k_smoothing"));
                    });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_out,
                           false, // inverse
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
