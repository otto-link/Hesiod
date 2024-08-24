/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

DiffusionLimitedAggregation::DiffusionLimitedAggregation(const ModelConfig *p_config)
    : BaseNode(p_config)
{
  LOG->trace("DiffusionLimitedAggregation::DiffusionLimitedAggregation");

  // model
  this->node_caption = "DiffusionLimitedAggregation";

  // inputs
  this->input_captions = {};
  this->input_types = {};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["scale"] = NEW_ATTR_FLOAT(0.01f, 0.005f, 0.1f, "%.3f");

  this->attr["seeding_radius"] = NEW_ATTR_FLOAT(0.4f, 0.1f, 0.5f);
  this->attr["seeding_outer_radius_ratio"] = NEW_ATTR_FLOAT(0.2f, 0.01f, 0.5f);
  this->attr["slope"] = NEW_ATTR_FLOAT(8.f, 0.1f, 32.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"seed",
                            "scale",
                            "seeding_radius",
                            "seeding_outer_radius_ratio",
                            "slope",
                            "noise_ratio",
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
  this->description = "DiffusionLimitedAggregation creates branching, fractal-like "
                      "structures that resemble the rugged, irregular contours of "
                      "mountain ranges.";

  // clang-format off
  this->input_descriptions = {};
  this->output_descriptions = {"DiffusionLimitedAggregation heightmap."};

  this->attribute_descriptions["seed"] = "The seed for the random number generator.";
  this->attribute_descriptions["scale"] = "A scaling factor that influences the density of the particles in the DLA pattern.";
  this->attribute_descriptions["seeding_radius"] = "The radius within which initial seeding of particles occurs. This radius defines the area where the first particles are placed.";
  this->attribute_descriptions["seeding_outer_radius_ratio"] = "The ratio between the outer seeding radius and the initial seeding radius. It determines the outer boundary for particle seeding.";
  this->attribute_descriptions["slope"] = "Slope of the talus added to the DLA pattern.";
  this->attribute_descriptions["noise_ratio"] = " A parameter that controls the amount of randomness or noise introduced in the talus formation process.";
  // clang-format on
}

std::shared_ptr<QtNodes::NodeData> DiffusionLimitedAggregation::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void DiffusionLimitedAggregation::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                                            QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void DiffusionLimitedAggregation::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::Array array = hmap::diffusion_limited_aggregation(
      p_config->shape,
      GET_ATTR_FLOAT("scale"),
      GET_ATTR_SEED("seed"),
      GET_ATTR_FLOAT("seeding_radius"),
      GET_ATTR_FLOAT("seeding_outer_radius_ratio"),
      GET_ATTR_FLOAT("slope"),
      GET_ATTR_FLOAT("noise_ratio"));

  p_out->from_array_interp_nearest(array);

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
