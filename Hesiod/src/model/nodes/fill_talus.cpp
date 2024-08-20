/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

FillTalus::FillTalus(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("FillTalus::FillTalus");

  // model
  this->node_caption = "FillTalus";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["slope"] = NEW_ATTR_FLOAT(4.f, 0.1f, 16.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr_ordered_key = {"slope", "noise_ratio", "seed"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Fill the heightmap starting from the highest elevations using a "
                      "regular downslope.";

  // clang-format off
  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions["slope"] = "Downslope used for filling.";
  this->attribute_descriptions["noise_ratio"] = "Noise ratio, added to the downslope.";
  this->attribute_descriptions["seed"] = "Random seed number.";
  // clang-format on
}

std::shared_ptr<QtNodes::NodeData> FillTalus::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void FillTalus::setInData(std::shared_ptr<QtNodes::NodeData> data,
                          QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void FillTalus::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    float talus = GET_ATTR_FLOAT("slope") / (float)p_out->shape.x;

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [this, &talus](hmap::Array &x) {
                      hmap::fill_talus(x,
                                       talus,
                                       GET_ATTR_SEED("seed"),
                                       GET_ATTR_FLOAT("noise_ratio"));
                    });

    p_out->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
