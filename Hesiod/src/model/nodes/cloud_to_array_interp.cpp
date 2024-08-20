/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/interpolate.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CloudToArrayInterp::CloudToArrayInterp(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("CloudToArrayInterp::CloudToArrayInterp");

  // model
  this->node_caption = "CloudToArrayInterp";

  // inputs
  this->input_captions = {"cloud", "dx", "dy"};
  this->input_types = {CloudData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"heightmap"};
  this->output_types = {HeightMapData().type()};

  // no attributes

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "CloudToArrayInterp generates a smooth and continuous 2D elevation "
                      "map from a set of scattered points using Delaunay linear "
                      "interpolation.";

  this->input_descriptions = {
      "Set of points (x, y) and elevations z.",
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction)."};
  this->output_descriptions = {"Interpolated heightmap."};
}

std::shared_ptr<QtNodes::NodeData> CloudToArrayInterp::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CloudToArrayInterp::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->in = std::dynamic_pointer_cast<CloudData>(data);
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

void CloudToArrayInterp::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Cloud *p_cloud = HSD_GET_POINTER(this->in);

  if (p_cloud)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    if (p_cloud->get_npoints() > 0)
    {
      hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
      hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);

      hmap::fill(*p_out,
                 p_dx,
                 p_dy,
                 [this, p_cloud](hmap::Vec2<int>   shape,
                                 hmap::Vec4<float> bbox,
                                 hmap::Array      *p_noise_x,
                                 hmap::Array      *p_noise_y)
                 {
                   hmap::Array       array(shape);
                   hmap::Vec4<float> bbox_points = {0.f, 1.f, 0.f, 1.f};

                   p_cloud->to_array_interp(array,
                                            bbox_points,
                                            hmap::InterpolationMethod2D::DELAUNAY,
                                            p_noise_x,
                                            p_noise_y,
                                            bbox);
                   return array;
                 });
    }
    else
      // fill with zeroes
      hmap::transform(*p_out, [](hmap::Array array) { array = 0.f; });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
