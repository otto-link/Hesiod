/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/gradient_talus.hpp"


namespace hesiod
{

GradientTalus::GradientTalus(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("GradientTalus::GradientTalus");

  // model
  this->node_caption = "GradientTalus";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE(hmap::Vec2<float>(0.f, 1.f), 0.f, 1.f);

  this->attr_ordered_key = {"remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Gradient talus refers to the local steepest downslope, or the "
                      "direction in which the terrain descends most rapidly, at each "
                      "point on the heightmap.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Gradient talus."};
}

std::shared_ptr<QtNodes::NodeData> GradientTalus::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void GradientTalus::setInData(std::shared_ptr<QtNodes::NodeData> data,
                              QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void GradientTalus::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    hmap::transform(*p_out,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in)
                    { out = hmap::gradient_talus(in); });

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
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
