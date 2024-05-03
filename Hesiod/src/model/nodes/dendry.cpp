/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Dendry::Dendry(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Dendry::Dendry");

  // model
  this->node_caption = "Dendry";

  // inputs
  this->input_captions = {"control", "dx", "dy", "envelope"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["subsampling"] = NEW_ATTR_INT(4, 1, 8);

  this->attr["kw"] = NEW_ATTR_WAVENB(hmap::Vec2<float>(8.f, 8.f));
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["eps"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["resolution"] = NEW_ATTR_INT(1, 1, 8);
  this->attr["displacement"] = NEW_ATTR_FLOAT(0.075f, 0.f, 0.2f);
  this->attr["primitives_resolution_steps"] = NEW_ATTR_INT(3, 1, 8);
  this->attr["slope_power"] = NEW_ATTR_FLOAT(1.f, 0.f, 2.f);
  this->attr["noise_amplitude_proportion"] = NEW_ATTR_FLOAT(0.01f, 0.f, 1.f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"subsampling",
                            "_SEPARATOR_",
                            "kw",
                            "seed",
                            "eps",
                            "slope_power",
                            "resolution",
                            "displacement",
                            "primitives_resolution_steps",
                            "noise_amplitude_proportion",
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
  this->description = "Dendry is a procedural model for dendritic patterns generation.";

  this->input_descriptions = {
      "Global control heightmap that defines the overall shape of the output.",
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction).",
      "Output noise amplitude envelope."};
  this->output_descriptions = {"Generated noise."};

  this->attribute_descriptions
      ["subsampling"] = "Function evaluation subsampling, use higher values for faster "
                        "computation at the cost of a coarser resolution.";
  this->attribute_descriptions["kw"] = "Noise wavenumbers (kx, ky) for each directions.";
  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions
      ["eps"] = "Used to bias the area where points are generated in cells.";
  this->attribute_descriptions["slope_power"] = "Additional parameter to control the "
                                                "variation of slope on terrains.";
  this->attribute_descriptions
      ["resolution"] = "Number of resolutions in the noise function.";
  this->attribute_descriptions["displacement"] = "Maximum displacement of segments.";
  this->attribute_descriptions["primitives_resolution_steps"] =
      "Additional resolution steps in the primitive resolution.";
  this->attribute_descriptions["noise_amplitude_proportion"] =
      "Proportion of the amplitude of the control function as noise.";
}

std::shared_ptr<QtNodes::NodeData> Dendry::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Dendry::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->control_function = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->dx = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->dy = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 3:
    this->envelope = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Dendry::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  // base noise function
  hmap::HeightMap *p_ctrl = HSD_GET_POINTER(this->control_function);
  hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
  hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);
  hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_ctrl)
  {
    // TODO shape
    hmap::Array ctrl_array = p_ctrl->to_array({128, 128});

    hmap::fill(*p_out,
               p_dx,
               p_dy,
               [this, &ctrl_array](hmap::Vec2<int>   shape,
                                   hmap::Vec4<float> bbox,
                                   hmap::Array      *p_noise_x,
                                   hmap::Array      *p_noise_y)
               {
                 return hmap::dendry(shape,
                                     GET_ATTR_WAVENB("kw"),
                                     GET_ATTR_SEED("seed"),
                                     ctrl_array,
                                     GET_ATTR_FLOAT("eps"),
                                     GET_ATTR_INT("resolution"),
                                     GET_ATTR_FLOAT("displacement"),
                                     GET_ATTR_INT("primitives_resolution_steps"),
                                     GET_ATTR_FLOAT("slope_power"),
                                     GET_ATTR_FLOAT("noise_amplitude_proportion"),
                                     true, // add noise
                                     0.5f, // overlap
                                     p_noise_x,
                                     p_noise_y,
                                     nullptr,
                                     bbox,
                                     GET_ATTR_INT("subsampling"));
               });

    p_out->smooth_overlap_buffers();

    // add envelope
    if (p_env)
    {
      float hmin = p_out->min();
      hmap::transform(*p_out,
                      *p_env,
                      [&hmin](hmap::Array &a, hmap::Array &b)
                      {
                        a -= hmin;
                        a *= b;
                      });
    }

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
