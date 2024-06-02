/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Caldera::Caldera(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Caldera::Caldera");

  // model
  this->node_caption = "Caldera";

  // inputs
  this->input_captions = {"dr"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.25f, 0.01f, 1.f);
  this->attr["sigma_inner"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.3f, "%.3f");
  this->attr["sigma_outer"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.3f, "%.3f");
  this->attr["noise_r_amp"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.3f, "%.3f");
  this->attr["z_bottom"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["noise_ratio_z"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"radius",
                            "sigma_inner",
                            "sigma_outer",
                            "noise_r_amp",
                            "z_bottom",
                            "noise_ratio_z",
                            "center.x",
                            "center.y",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "Caldera generates a volcanic caldera landscape.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (normal direction)."};
  this->output_descriptions = {"Caldera heightmap."};

  this->attribute_descriptions["radius"] = "Crater radius.";
  this->attribute_descriptions["sigma_inner"] = "Crater inner lip half-width.";
  this->attribute_descriptions["sigma_outer"] = "Crater outer lip half-width.";
  this->attribute_descriptions
      ["noise_r_amp"] = "Noise amplitude for the radial displacement.";
  this->attribute_descriptions["z_bottom"] = "Crater bottom elevation.";
  this->attribute_descriptions
      ["noise_ratio_z"] = "Noise amplitude for the vertical displacement.";
  this->attribute_descriptions["center.x"] = "Center x coordinate.";
  this->attribute_descriptions["center.y"] = "Center y coordinate.";
}

std::shared_ptr<QtNodes::NodeData> Caldera::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Caldera::setInData(std::shared_ptr<QtNodes::NodeData> data,
                        QtNodes::PortIndex /* port_index */)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  this->dr = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Caldera::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  // base noise function
  hmap::HeightMap *p_dr = HSD_GET_POINTER(this->dr);
  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  float radius_pixel = std::max(1.f, GET_ATTR_FLOAT("radius") * p_out->shape.x);
  float sigma_inner_pixel = std::max(1.f, GET_ATTR_FLOAT("sigma_inner") * p_out->shape.x);
  float sigma_outer_pixel = std::max(1.f, GET_ATTR_FLOAT("sigma_outer") * p_out->shape.x);
  float noise_r_amp_pixel = std::max(1.f, GET_ATTR_FLOAT("noise_r_amp") * p_out->shape.x);

  hmap::fill(*p_out,
             p_dr,
             [this,
              &center,
              &radius_pixel,
              &sigma_inner_pixel,
              &sigma_outer_pixel,
              &noise_r_amp_pixel](hmap::Vec2<int>   shape,
                                  hmap::Vec4<float> bbox,
                                  hmap::Array      *p_noise)
             {
               return hmap::caldera(shape,
                                    radius_pixel,
                                    sigma_inner_pixel,
                                    sigma_outer_pixel,
                                    GET_ATTR_FLOAT("z_bottom"),
                                    p_noise,
                                    noise_r_amp_pixel,
                                    GET_ATTR_FLOAT("noise_ratio_z"),
                                    center,
                                    bbox);
             });

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

} // namespace hesiod
