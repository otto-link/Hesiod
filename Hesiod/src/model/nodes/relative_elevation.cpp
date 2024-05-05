/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

RelativeElevation::RelativeElevation(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("RelativeElevation::RelativeElevation");

  // model
  this->node_caption = "RelativeElevation";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.3f");

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "RelativeElevation identifies heightmap relative elevation based "
                      "on the surrounding heightmap values.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Relative elevation."};

  this->attribute_descriptions
      ["radius"] = "Filter radius with respect to the domain size.";
}

std::shared_ptr<QtNodes::NodeData> RelativeElevation::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void RelativeElevation::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                  QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void RelativeElevation::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();
    
    // copy the input heightmap
    *p_out = *p_in;
    
    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    [&ir](hmap::Array &x) { x = hmap::relative_elevation(x, ir); });

    p_out->smooth_overlap_buffers();
    p_out->remap();
    
    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
