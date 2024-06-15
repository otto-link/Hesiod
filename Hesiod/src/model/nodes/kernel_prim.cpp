/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

KernelPrim::KernelPrim(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("KernelPrim::KernelPrim");

  // model
  this->node_caption = "KernelPrim";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"kernel"};
  this->output_types = {KernelData().type()};

  // attributes
  this->attr["kernel"] = NEW_ATTR_MAPENUM(kernel_map, "cubic_pulse");
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.001f, 0.2f, "%.3f");
  this->attr["normalize"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"kernel", "radius", "normalize"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<KernelData>();
    this->compute();
  }

  // documentation
  this->description = ".";

  this->input_descriptions = {};
  this->output_descriptions = {
      "KernelPrim generates a 'kernel', refering to a small matrix used to apply "
      "specific effects based on convolution for instance."};

  this->attribute_descriptions["kernel"] = "Kernel type.";
  this->attribute_descriptions
      ["radius"] = "Kernel radius with respect to the domain size.";
  this->attribute_descriptions["normalize"] =
      "Normalize kernel so that the sum of the elements equals 1, preserving the overall "
      "intensity of an heightmap after convolution for instance.";
}

std::shared_ptr<QtNodes::NodeData> KernelPrim::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void KernelPrim::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                           QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void KernelPrim::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Array *p_out = this->out->get_ref();

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * this->p_config->shape.x));

  // kernel definition
  hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

  switch (GET_ATTR_MAPENUM("kernel"))
  {
  case Kernel::CONE:
    *p_out = hmap::cone(kernel_shape);
    break;

  case Kernel::CUBIC_PULSE:
    *p_out = hmap::cubic_pulse(kernel_shape);
    break;

  case Kernel::LORENTZIAN:
    *p_out = hmap::lorentzian(kernel_shape);
    break;

  case Kernel::SMOOTH_COSINE:
    *p_out = hmap::smooth_cosine(kernel_shape);
    break;
  }

  if (GET_ATTR_BOOL("normalize"))
    *p_out /= p_out->sum();

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
