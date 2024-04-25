/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

MaskToHeightMap::MaskToHeightMap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("MaskToHeightMap::MaskToHeightMap");

  // model
  this->node_caption = "MaskToHeightMap";

  // inputs
  this->input_captions = {"mask"};
  this->input_types = {MaskData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"inverse", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Convert a mask heightmap to an actual heightmap.";

  this->input_descriptions = {"Mask."};
  this->output_descriptions = {"Heightmap."};
}

std::shared_ptr<QtNodes::NodeData> MaskToHeightMap::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void MaskToHeightMap::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->mask = std::dynamic_pointer_cast<MaskData>(data);

  this->compute();
}

// --- computing

void MaskToHeightMap::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_mask)
  {
    // copy the input heightmap
    *p_out = *p_mask;

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
