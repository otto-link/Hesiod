/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Warp::Warp(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Warp::Warp");

  // model
  this->node_caption = "Warp";

  // inputs
  this->input_captions = {"input", "dx", "dy"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  // - empty

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Warp node transforms a base heightmap by warping/pushing "
                      "pixels as defined by the input displacements.";

  this->input_descriptions = {
      "Input heightmap.",
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction)."};
  this->output_descriptions = {"Warped heightmap."};
}

std::shared_ptr<QtNodes::NodeData> Warp::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Warp::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->dx = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->dy = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Warp::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

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

    hmap::warp(z_array, p_dx_array, p_dy_array);

    p_out->from_array_interp_nearest(z_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
