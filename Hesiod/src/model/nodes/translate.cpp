/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Translate::Translate(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Translate::Translate");

  // model
  this->node_caption = "Translate";

  // inputs
  this->input_captions = {"input", "dx", "dy"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["dx"] = NEW_ATTR_FLOAT(0.1f, -1.f, 1.f);
  this->attr["dy"] = NEW_ATTR_FLOAT(0.1f, -1.f, 1.f);
  this->attr["periodic"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"dx", "dy", "periodic"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "Translates an heightmap by a specified amount along the x and y "
                      "axes. This function shifts the contents of the input array by "
                      "`dx` and `dy` units along the x and y axes, respectively. It "
                      "supports both periodic boundary conditions, where the array wraps "
                      "around, and non-periodic conditions.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction).",
      "Control parameter, acts as a multiplier for the weight parameter."};
  this->output_descriptions = {"Translate heightmap."};

  this->attribute_descriptions["dx"] = "The translation distance along the x-axis. "
                                       "Positive values shift the array to the right.";
  this->attribute_descriptions["dy"] = "The translation distance along the y-axis. "
                                       "Positive values shift the array downward.";
  this->attribute_descriptions["periodic"] =
      "If set to `true`, the translation is periodic, meaning that elements that move "
      "out of one side of the array reappear on the opposite side.";
}

std::shared_ptr<QtNodes::NodeData> Translate::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Translate::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void Translate::compute()
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

    z_array = hmap::translate(z_array,
                              GET_ATTR_FLOAT("dx"),
                              GET_ATTR_FLOAT("dy"),
                              GET_ATTR_BOOL("periodic"),
                              p_dx_array,
                              p_dy_array);

    p_out->from_array_interp_nearest(z_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
