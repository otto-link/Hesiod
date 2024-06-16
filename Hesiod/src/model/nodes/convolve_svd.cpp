/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ConvolveSVD::ConvolveSVD(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ConvolveSVD::ConvolveSVD");

  // model
  this->node_caption = "ConvolveSVD";

  // inputs
  this->input_captions = {"input", "kernel"};
  this->input_types = {HeightMapData().type(), KernelData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["rank"] = NEW_ATTR_INT(4, 1, 8);

  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"rank", "_SEPARATOR_", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ConvolveSVD performs convolution using a Singular Value "
                      "Decomposition (SVD) of the kernel to accelerate the process.";

  this->input_descriptions = {"Input heightmap.", "Convolution kernel."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions["rank"] = "Rank of the singular value decomposition.";
}

std::shared_ptr<QtNodes::NodeData> ConvolveSVD::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ConvolveSVD::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->kernel = std::dynamic_pointer_cast<KernelData>(data);
  }

  this->compute();
}

// --- computing

void ConvolveSVD::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::Array     *p_kernel = HSD_GET_POINTER(this->kernel);

  if (p_in && p_kernel)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    hmap::transform(*p_out,
                    *p_in,
                    [this, p_kernel](hmap::Array &out, hmap::Array &in)
                    { out = hmap::convolve2d_svd(in, *p_kernel, GET_ATTR_INT("rank")); });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_out,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR_BOOL("remap"),
                           GET_ATTR_RANGE("remap_range"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
