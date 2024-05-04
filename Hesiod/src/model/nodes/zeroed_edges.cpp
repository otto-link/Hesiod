/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/zeroed_edges.hpp"


namespace hesiod
{

ZeroedEdges::ZeroedEdges(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ZeroedEdges::ZeroedEdges");

  // model
  this->node_caption = "ZeroedEdges";

  // inputs
  this->input_captions = {"input", "dr"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.25f, 0.f, 0.5f, "%.2f");
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"sigma", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ZeroedEdges is an operator that enforces values close to zero at "
                      "the domain edges.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["sigma"] = "Shape half-width.";
}

std::shared_ptr<QtNodes::NodeData> ZeroedEdges::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ZeroedEdges::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->dr = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void ZeroedEdges::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_dr = HSD_GET_POINTER(this->dr);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    float sigma = GET_ATTR_FLOAT("sigma");

    if (!p_dr)
      hmap::transform(*p_out,
                      [this, &sigma](hmap::Array &z, hmap::Vec4<float> bbox)
                      { hmap::zeroed_edges(z, sigma, nullptr, bbox); });

    else
      hmap::transform(
          *p_out,
          *p_dr,
          [this, &sigma](hmap::Array &z, hmap::Array &dr, hmap::Vec4<float> bbox)
          { hmap::zeroed_edges(z, sigma, &dr, bbox); });

    if (GET_ATTR_BOOL("remap"))
      p_out->remap(GET_ATTR_RANGE("remap_range").x, GET_ATTR_RANGE("remap_range").y);
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
