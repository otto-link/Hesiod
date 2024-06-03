/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathSDF::PathSDF(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("PathSDF::PathSDF");

  // model
  this->node_caption = "PathSDF";

  // inputs
  this->input_captions = {"path", "dx", "dy"};
  this->input_types = {PathData().type(), HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"sdf"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["inverse"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"remap", "inverse"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "PathSDF evaluates the signed distance function of a polyline. It "
                      "assigns a signed distance value to every point in space. For "
                      "points outside the polyline, the distance is positive, while for "
                      "points inside, it's negative. The zero level set of this function "
                      "precisely defines the polyline's path Project path points to an "
                      "heightmap.";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Signed distance as an heightmap."};
}

std::shared_ptr<QtNodes::NodeData> PathSDF::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathSDF::setInData(std::shared_ptr<QtNodes::NodeData> data,
                        QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<PathData>(data);
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

void PathSDF::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Path      *p_path = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
  hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);

  if (p_path)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    if (p_path->get_npoints() > 1)
    {
      hmap::fill(
          *p_out,
          p_dx,
          p_dy,
          [this, p_path](hmap::Vec2<int>   shape,
                         hmap::Vec4<float> bbox,
                         hmap::Array      *p_noise_x,
                         hmap::Array      *p_noise_y)
          {
            hmap::Vec4<float> bbox_full = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
            return p_path->to_array_sdf(shape, bbox_full, p_noise_x, p_noise_y, bbox);
          });

      // post-process
      post_process_heightmap(*p_out,
                             GET_ATTR_BOOL("inverse"),
                             false, // smoothing,
                             0,
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             GET_ATTR_BOOL("remap"),
                             {0.f, 1.f});
    }
    else
      // fill with zeros
      hmap::transform(*p_out, [](hmap::Array x) { x = 0.f; });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
