/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HeightmapToKernel::HeightmapToKernel(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("HeightmapToKernel::HeightmapToKernel");

  // model
  this->node_caption = "HeightmapToKernel";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"kernel"};
  this->output_types = {KernelData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.001f, 0.2f, "%.3f");
  this->attr["normalize"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"radius", "normalize"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<KernelData>();
    // create a dummy 8x8 array to allow preview even if it's only zeroes
    *this->out->get_ref() = hmap::Array(hmap::Vec2<int>(8, 8));

    this->compute();
  }

  // documentation
  this->description = "Convert an heightmap to a kernel.";

  this->input_descriptions = {"Heightmap."};
  this->output_descriptions = {"Kernel."};

  this->attribute_descriptions
      ["radius"] = "Kernel radius with respect to the domain size.";
  this->attribute_descriptions["normalize"] =
      "Normalize kernel so that the sum of the elements equals 1, preserving the overall "
      "intensity of an heightmap after convolution for instance.";
}

std::shared_ptr<QtNodes::NodeData> HeightmapToKernel::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void HeightmapToKernel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                  QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void HeightmapToKernel::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Array *p_out = this->out->get_ref();

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * this->p_config->shape.x));
    hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

    hmap::Array array = p_in->to_array();
    *p_out = array.resample_to_shape(kernel_shape);

    if (GET_ATTR_BOOL("normalize"))
      *p_out /= p_out->sum();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
