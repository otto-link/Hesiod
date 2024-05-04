/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/select_gt.hpp"


namespace hesiod
{

SelectGt::SelectGt(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SelectGt::SelectGt");

  // model
  this->node_caption = "SelectGt";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["value"] = NEW_ATTR_FLOAT(0.5f, -1.f, 1.f);
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");

  this->attr_ordered_key = {"value",
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
  this->description = "SelectGt is a thresholding operator. It transforms an input "
                      "heightmap into a binary heightmap, where each pixel is assigned "
                      "either a value of 0 or 1, depending on whether its intensity "
                      "value surpasses a specified threshold value.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Mask heightmap (in [0, 1])."};

  this->attribute_descriptions["value"] = "Selection value.";
}

std::shared_ptr<QtNodes::NodeData> SelectGt::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SelectGt::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void SelectGt::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    hmap::transform(*p_out,
                    *p_in,
                    [this](hmap::Array &out, hmap::Array &in)
                    { out = hmap::select_gt(in, GET_ATTR_FLOAT("value")); });

    // post-process
    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           GET_ATTR_BOOL("smoothing"),
                           GET_ATTR_FLOAT("smoothing_radius"),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           true, // force remap
                           {0.f, 1.f});
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
