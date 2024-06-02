/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SelectTransitions::SelectTransitions(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SelectTransitions::SelectTransitions");

  // model
  this->node_caption = "SelectTransitions";

  // inputs
  this->input_captions = {"input 1", "input 2", "blend"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");

  this->attr_ordered_key = {"inverse", "smoothing", "smoothing_radius"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "SelectTransitions returns a mask filled with 1 at the blending "
                      "transition between two heightmaps, and 0 elsewhere.";

  this->input_descriptions = {"Input heightmap 1.",
                              "Input heightmap 2.",
                              "Blended heightmap."};
  this->output_descriptions = {"Mask heightmap (in [0, 1])."};
}

std::shared_ptr<QtNodes::NodeData> SelectTransitions::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SelectTransitions::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    break;
  case 2:
    this->blend = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void SelectTransitions::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMap *p_in2 = HSD_GET_POINTER(this->in2);
  hmap::HeightMap *p_blend = HSD_GET_POINTER(this->blend);

  if (p_in1 && p_in2 && p_blend)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    hmap::transform(*p_out,
                    *p_in1,
                    *p_in2,
                    *p_blend,
                    [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2, hmap::Array &a3)
                    { m = hmap::select_transitions(a1, a2, a3); });

    // post-process
    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           GET_ATTR_BOOL("smoothing"),
                           GET_ATTR_FLOAT("smoothing_radius"),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           false, // remap
                           {0.f, 0.f});

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
