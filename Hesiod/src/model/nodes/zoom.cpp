/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Zoom::Zoom(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Zoom::Zoom");

  // model
  this->node_caption = "Zoom";

  // inputs
  this->input_captions = {"input", "dx", "dy"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["zoom_factor"] = NEW_ATTR_FLOAT(2.f, 1.f, 10.f);
  this->attr["periodic"] = NEW_ATTR_BOOL(false);

  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"zoom_factor", "periodic", "center.x", "center.y"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "Applies a zoom effect to an heightmap with an adjustable center. "
                      "This function scales the input 2D array by a specified zoom "
                      "factor, effectively resizing the array's contents. The zoom "
                      "operation is centered around a specified point within the array, "
                      "allowing for flexible zooming behavior.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction).",
      "Control parameter, acts as a multiplier for the weight parameter."};
  this->output_descriptions = {"Zoom heightmap."};

  this->attribute_descriptions
      ["zoom_factor"] = "The factor by which to zoom the heightmap.";
  this->attribute_descriptions["periodic"] = "If set to `true`, the zoom is periodic.";
  this->attribute_descriptions["center.x"] = "Center of the zoom operation.";
  this->attribute_descriptions["center.y"] = "Center of the zoom operation.";
}

std::shared_ptr<QtNodes::NodeData> Zoom::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Zoom::setInData(std::shared_ptr<QtNodes::NodeData> data,
                     QtNodes::PortIndex                 port_index)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->dx = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->dy = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Zoom::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
    hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);
    hmap::HeightMap *p_out = this->out->get_ref();

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    hmap::Array  dx_array, dy_array;
    hmap::Array *p_dx_array = nullptr, *p_dy_array = nullptr;

    if (p_dx)
    {
      dx_array = p_dx->to_array();
      p_dx_array = &dx_array;
    }

    if (p_dy)
    {
      dy_array = p_dy->to_array();
      p_dy_array = &dy_array;
    }

    hmap::Vec2<float> center;
    center.x = GET_ATTR_FLOAT("center.x");
    center.y = GET_ATTR_FLOAT("center.y");

    z_array = hmap::zoom(z_array,
                         GET_ATTR_FLOAT("zoom_factor"),
                         GET_ATTR_BOOL("periodic"),
                         center,
                         p_dx_array,
                         p_dy_array);

    p_out->from_array_interp_nearest(z_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
