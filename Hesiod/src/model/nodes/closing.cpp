/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/closing.hpp"


namespace hesiod
{

Closing::Closing(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Closing::Closing");

  // model
  this->node_caption = "Closing";

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
  this->description = "Closing is a combination of dilation followed by erosion. It is "
                      "primarily used for closing small holes or gaps in objects. Use "
                      "Cases: Hole filling: Closing can fill small holes or gaps in "
                      "objects, making them more solid and complete. Connecting broken "
                      "objects: Closing can help connect broken segments or regions in "
                      "an image, making objects more continuous.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Dilated heightmap."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
}

std::shared_ptr<QtNodes::NodeData> Closing::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Closing::setInData(std::shared_ptr<QtNodes::NodeData> data,
                        QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Closing::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    *p_in,
                    [&ir](hmap::Array &out, hmap::Array &in)
                    { out = hmap::closing(in, ir); });
    p_out->smooth_overlap_buffers();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
