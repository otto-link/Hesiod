/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ExpandShrink::ExpandShrink(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ExpandShrink::ExpandShrink");

  // model
  this->node_caption = "ExpandShrink";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["kernel"] = NEW_ATTR_MAPENUM(kernel_type_map, "cubic_pulse");
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.2f, "%.3f");
  this->attr["shrink"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"kernel", "radius", "shrink"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ExpandShrink is a maximum/minimum filter with a weighted kernel. "
                      "It can be used to enhance or extract features while preserving "
                      "the essential structure of the heightmap.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["kernel"] = "Weighting kernel.";
  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
  this->attribute_descriptions["shrink"] = "Shrink (if true) or expand (if false).";
}

std::shared_ptr<QtNodes::NodeData> ExpandShrink::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ExpandShrink::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void ExpandShrink::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    // kernel definition
    hmap::Array     kernel_array;
    hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

    kernel_array = hmap::get_kernel(kernel_shape,
                                    (hmap::KernelType)GET_ATTR_MAPENUM("kernel"));

    // core operator
    std::function<void(hmap::Array &, hmap::Array *)> lambda;

    if (GET_ATTR_BOOL("shrink"))
      lambda = [&kernel_array](hmap::Array &x, hmap::Array *p_mask)
      { hmap::shrink(x, kernel_array, p_mask); };
    else
      lambda = [&kernel_array](hmap::Array &x, hmap::Array *p_mask)
      { hmap::expand(x, kernel_array, p_mask); };

    hmap::transform(*p_out, p_mask, lambda);
    p_out->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
