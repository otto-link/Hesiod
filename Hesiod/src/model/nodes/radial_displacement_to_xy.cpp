/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

RadialDisplacementToXy::RadialDisplacementToXy(const ModelConfig *p_config)
    : BaseNode(p_config)
{
  LOG->trace("RadialDisplacementToXy::RadialDisplacementToXy");

  // model
  this->node_caption = "RadialDisplacementToXy";

  // inputs
  this->input_captions = {"dr"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"dx", "dy"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};

  // attributes
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"center.x", "center.y"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->dx = std::make_shared<HeightMapData>(p_config);
    this->dy = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "RadialDisplacementToXy interprets the input array dr as a radial "
                      "displacement and convert it to a pair of displacements dx and dy "
                      "in cartesian coordinates.";

  this->input_descriptions = {"Radial displacement."};
  this->output_descriptions = {"Displacement for  the x-direction.",
                               "Displacement for  the y-direction."};

  this->attribute_descriptions["center.x"] = "Center x coordinate.";
  this->attribute_descriptions["center.y"] = "Center y coordinate.";
}

std::shared_ptr<QtNodes::NodeData> RadialDisplacementToXy::outData(
    QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->dx);
  case 1:
  default:
    return std::static_pointer_cast<QtNodes::NodeData>(this->dy);
  }
}

void RadialDisplacementToXy::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                       QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->dr = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void RadialDisplacementToXy::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_dr = HSD_GET_POINTER(this->dr);

  if (p_dr)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_dx = this->dx->get_ref();
    hmap::HeightMap *p_dy = this->dy->get_ref();

    hmap::Vec2<float> center;
    center.x = GET_ATTR_FLOAT("center.x");
    center.y = GET_ATTR_FLOAT("center.y");

    hmap::transform(*p_dr,
                    *p_dx,
                    *p_dy,
                    [&center](hmap::Array      &dr,
                              hmap::Array      &dx,
                              hmap::Array      &dy,
                              hmap::Vec4<float> bbox)
                    { hmap::radial_displacement_to_xy(dr, dx, dy, center, bbox); });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
