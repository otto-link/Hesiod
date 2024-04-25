/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Dilation::Dilation(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Dilation::Dilation");

  // model
  this->node_caption = "Dilation";

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
  this->description =
      "Dilation expands the boundaries of objects in an image and fills in small gaps or "
      "holes in those objects. Use Cases: (+) Filling gaps: Dilation can be used to fill "
      "small gaps or holes in objects, making them more solid. (+) Merging objects: "
      "Dilation can help merge nearby objects or connect broken segments in an image.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Dilated heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";

  LOG_DEBUG("\n%s", this->get_full_description().c_str());
}

std::shared_ptr<QtNodes::NodeData> Dilation::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Dilation::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Dilation::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out, [&ir](hmap::Array &x) { x = hmap::dilation(x, ir); });
    p_out->smooth_overlap_buffers();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
