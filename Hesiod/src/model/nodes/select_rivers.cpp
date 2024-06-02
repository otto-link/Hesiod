/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SelectRivers::SelectRivers(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SelectRivers::SelectRivers");

  // model
  this->node_caption = "SelectRivers";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["talus_ref"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 10.f);
  this->attr["clipping_ratio"] = NEW_ATTR_FLOAT(50.f, 0.1f, 100.f);
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");
  this->attr["remap"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"talus_ref",
                            "clipping_ratio",
                            "_SEPARATOR_",
                            "inverse",
                            "smoothing",
                            "smoothing_radius",
                            "remap"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "SelectRivers is a thresholding operator. It creates a mask for "
                      "river systems based on a flow accumulation threshold.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Mask heightmap (in [0, 1])."};

  // this->attribute_descriptions[""] = ".";
}

std::shared_ptr<QtNodes::NodeData> SelectRivers::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SelectRivers::setInData(std::shared_ptr<QtNodes::NodeData> data,
                             QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void SelectRivers::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // work on a single array as a temporary solution
    hmap::Array z_array = p_in->to_array();
    z_array = hmap::select_rivers(z_array,
                                  GET_ATTR_FLOAT("talus_ref"),
                                  GET_ATTR_FLOAT("clipping_ratio"));
    p_out->from_array_interp(z_array);

    // post-process
    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           GET_ATTR_BOOL("smoothing"),
                           GET_ATTR_FLOAT("smoothing_radius"),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR_BOOL("remap"),
                           {0.f, 1.f});
    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
