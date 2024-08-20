/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/transform.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

WarpDownslope::WarpDownslope(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("WarpDownslope::WarpDownslope");

  // model
  this->node_caption = "WarpDownslope";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["amount"] = NEW_ATTR_FLOAT(0.02f, 0.f, 1.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.3f");
  this->attr["reverse"] = NEW_ATTR_BOOL(true);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Warp the heightmap with a direction and amount based on the local "
                      "downslope.";

  // clang-format off
  this->input_descriptions = {"Input heightmap.",
			      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["amount"] = "Warp scaling with respect to the gradient.";
  this->attribute_descriptions["radius"] = "Prefiltering radius for gradient computation.";
  this->attribute_descriptions["reverse"] = "Reverse warping direction (upslope if true and downslope otherwise).";
  // clang-format on
}

std::shared_ptr<QtNodes::NodeData> WarpDownslope::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void WarpDownslope::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void WarpDownslope::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();

    int ir = (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x);

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::Array  mask_array;
    hmap::Array *p_mask_array = nullptr;

    if (p_mask)
    {
      mask_array = p_mask->to_array();
      p_mask_array = &mask_array;
    }

    hmap::warp_downslope(z_array,
                         p_mask_array,
                         GET_ATTR_FLOAT("amount"),
                         ir,
                         GET_ATTR_BOOL("reverse"));

    p_out->from_array_interp_nearest(z_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
