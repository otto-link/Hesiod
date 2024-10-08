/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/morphology.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Opening::Opening(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Opening::Opening");

  // model
  this->node_caption = "Opening";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.01f, 0.f, 0.05f, "%.3f");

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Opening is a combination of erosion followed by dilation. It is "
                      "primarily used for removing noise while preserving the shape and "
                      "size of objects. Use Cases: Noise removal: Opening is effective "
                      "in removing small, isolated noise regions while keeping the main "
                      "objects intact. Smoothing object boundaries: Opening can help "
                      "smooth the boundaries of objects in an image while preserving "
                      "their overall shape.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Dilated heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
}

std::shared_ptr<QtNodes::NodeData> Opening::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Opening::setInData(std::shared_ptr<QtNodes::NodeData> data,
                        QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Opening::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    *p_in,
                    [&ir](hmap::Array &out, hmap::Array &in)
                    { out = hmap::opening(in, ir); });
    p_out->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
