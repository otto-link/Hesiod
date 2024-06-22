/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

KernelGabor::KernelGabor(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("KernelGabor::KernelGabor");

  // model
  this->node_caption = "KernelGabor";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"kernel"};
  this->output_types = {KernelData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.001f, 0.2f, "%.3f");
  this->attr["normalize"] = NEW_ATTR_BOOL(false);

  this->attr["kw"] = NEW_ATTR_FLOAT(2.f, 0.01f, 32.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);

  this->attr_ordered_key = {"radius", "normalize", "_SEPARATOR_", "kw", "angle"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<KernelData>();
    this->compute();
  }

  // documentation
  this->description = ".";

  this->input_descriptions = {};
  this->output_descriptions = {"KernelGabor generates a Gabor kernel."};

  this->attribute_descriptions
      ["radius"] = "Kernel radius with respect to the domain size.";
  this->attribute_descriptions["normalize"] =
      "Normalize kernel so that the sum of the elements equals 1, preserving the overall "
      "intensity of an heightmap after convolution for instance.";

  this->attribute_descriptions["kw"] = "Kernel wavenumber (spatial frequency).";
  this->attribute_descriptions["angle"] = "Kernel angle.";
}

std::shared_ptr<QtNodes::NodeData> KernelGabor::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void KernelGabor::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                            QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void KernelGabor::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Array *p_out = this->out->get_ref();

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * this->p_config->shape.x));

  // kernel definition
  hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

  *p_out = hmap::gabor(kernel_shape, GET_ATTR_FLOAT("kw"), GET_ATTR_FLOAT("angle"));

  if (GET_ATTR_BOOL("normalize"))
    *p_out /= p_out->sum();

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
