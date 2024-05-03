/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

StratifyOblique::StratifyOblique(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("StratifyOblique::StratifyOblique");

  // model
  this->node_caption = "StratifyOblique";

  // inputs
  this->input_captions = {"input", "noise", "mask"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["n_strata"] = NEW_ATTR_INT(3, 1, 16);
  this->attr["strata_noise"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["gamma"] = NEW_ATTR_FLOAT(0.7f, 0.01f, 5.f, "%.3f");
  this->attr["gamma_noise"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["talus_global"] = NEW_ATTR_FLOAT(1.f, 0.f, 8.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(30.f, -180.f, 180.f);

  this->attr_ordered_key = {"seed",
                            "n_strata",
                            "strata_noise",
                            "gamma",
                            "gamma_noise",
                            "talus_global",
                            "angle"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "StratifyOblique adds oblique stratifications to the input "
                      "heightmap.";

  this->input_descriptions = {
      "Input heightmap.",
      "Local elevation noise, value range expected to be scaled with the one of the "
      "input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Eroded heightmap."};

  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["n_strata"] = "Numbner of strata.";
  this->attribute_descriptions["strata_noise"] = "Noise range for the strata elevations.";
  this->attribute_descriptions
      ["gamma"] = "Reference value for the gamma correction applied to each strata, "
                  "influence the cliff elevation profile.";
  this->attribute_descriptions["gamma_noise"] = "Noise range for the gamma value.";
  this->attribute_descriptions["talus_global"] = "Step slope.";
  this->attribute_descriptions["angle"] = "Angle in the horizontal plane.";
}

std::shared_ptr<QtNodes::NodeData> StratifyOblique::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void StratifyOblique::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->noise = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void StratifyOblique::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_noise = HSD_GET_POINTER(this->noise);
  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    float zmin = p_out->min();
    float zmax = p_out->max();

    auto hs = hmap::linspace_jitted(zmin,
                                    zmax,
                                    GET_ATTR_INT("n_strata"),
                                    GET_ATTR_FLOAT("strata_noise"),
                                    GET_ATTR_SEED("seed"));

    float gmin = std::max(0.01f,
                          GET_ATTR_FLOAT("gamma") *
                              (1.f - GET_ATTR_FLOAT("gamma_noise")));
    float gmax = GET_ATTR_FLOAT("gamma") * (1.f + GET_ATTR_FLOAT("gamma_noise"));

    auto gs = hmap::random_vector(gmin,
                                  gmax,
                                  GET_ATTR_INT("n_strata") - 1,
                                  GET_ATTR_SEED("seed"));

    float talus = GET_ATTR_FLOAT("talus_global") / (float)p_out->shape.x;

    hmap::transform(*p_out,
                    p_mask,
                    p_noise,
                    [this, &hs, &gs, &talus](hmap::Array &h_out,
                                             hmap::Array *p_mask_array,
                                             hmap::Array *p_noise_array)
                    {
                      hmap::stratify_oblique(h_out,
                                             p_mask_array,
                                             hs,
                                             gs,
                                             talus,
                                             GET_ATTR_FLOAT("angle"),
                                             p_noise_array);
                    });

    p_out->smooth_overlap_buffers();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
