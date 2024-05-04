/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/recast_cliff.hpp"


namespace hesiod
{

RecastCliff::RecastCliff(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("RecastCliff::RecastCliff");

  // model
  this->node_caption = "RecastCliff";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["talus_global"] = NEW_ATTR_FLOAT(1.f, 0.f, 5.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 0.5f);
  this->attr["amplitude"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["gain"] = NEW_ATTR_FLOAT(2.f, 0.01f, 10.f);

  this->attr_ordered_key = {"talus_global", "radius", "amplitude", "gain"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "RecastCliff add cliffs in a heightmap by introducing sharp "
                      "changes in elevation to simulate steep vertical terrain features "
                      "such as cliffs or escarpments.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions
      ["talus_global"] = "Reference talus at which the cliff are added.";
  this->attribute_descriptions["radius"] = "Filter influence radius.";
  this->attribute_descriptions["amplitude"] = "Cliff amplitude.";
  this->attribute_descriptions["gain"] = "Gain, influence the cliff elevation profile.";
}

std::shared_ptr<QtNodes::NodeData> RecastCliff::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void RecastCliff::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void RecastCliff::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET_ATTR_FLOAT("talus_global") / (float)p_out->shape.x;
    int   ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    p_mask,
                    [this, &talus, &ir](hmap::Array &z, hmap::Array *p_mask)
                    {
                      hmap::recast_cliff(z,
                                         talus,
                                         ir,
                                         GET_ATTR_FLOAT("amplitude"),
                                         p_mask,
                                         GET_ATTR_FLOAT("gain"));
                    });

    p_out->smooth_overlap_buffers();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
