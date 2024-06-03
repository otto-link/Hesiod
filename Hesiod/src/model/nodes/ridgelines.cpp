/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Ridgelines::Ridgelines(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Ridgelines::Ridgelines");

  // model
  this->node_caption = "Ridgelines";

  // inputs
  this->input_captions = {"path", "dx", "dy"};
  this->input_types = {PathData().type(), HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"heightmap"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["talus_global"] = NEW_ATTR_FLOAT(4.f, -16.f, 16.f);
  this->attr["k_smoothing"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["width"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["vmin"] = NEW_ATTR_FLOAT(0.f, -1.f, 1.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Ridgelines generates an heightmap assuming the input path defines "
                      "a ridgeline.";

  this->input_descriptions = {
      "Set of points (x, y) and elevations z.",
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction)."};
  this->output_descriptions = {"Interpolated heightmap."};

  this->attribute_descriptions["talus_global"] = "Ridge slope.";
  this->attribute_descriptions["k_smoothing"] = "Smoothing intensity.";
  this->attribute_descriptions["width"] = "Ridge edge width.";
  this->attribute_descriptions["vmin"] = "Minimum value (lower values are clamped).";
}

std::shared_ptr<QtNodes::NodeData> Ridgelines::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Ridgelines::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void Ridgelines::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Path *p_path = HSD_GET_POINTER(this->in);

  if (p_path)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    if (p_path->get_npoints() > 1)
    {
      hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
      hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);

      std::vector<float> xs, ys, zs = {};

      for (size_t k = 0; k < p_path->get_npoints() - 1; k++)
      {
        LOG->trace("%ld", k);
        xs.push_back(p_path->points[k].x);
        ys.push_back(p_path->points[k].y);
        zs.push_back(p_path->points[k].v);

        xs.push_back(p_path->points[k + 1].x);
        ys.push_back(p_path->points[k + 1].y);
        zs.push_back(p_path->points[k + 1].v);
      }

      hmap::fill(*p_out,
                 p_dx,
                 p_dy,
                 [this, p_path, xs, ys, zs](hmap::Vec2<int>   shape,
                                            hmap::Vec4<float> bbox,
                                            hmap::Array      *p_noise_x,
                                            hmap::Array      *p_noise_y)
                 {
                   hmap::Array       array(shape);
                   hmap::Vec4<float> bbox_points = {0.f, 1.f, 0.f, 1.f};

                   array = hmap::ridgelines(shape,
                                            xs,
                                            ys,
                                            zs,
                                            GET_ATTR_FLOAT("talus_global"),
                                            GET_ATTR_FLOAT("k_smoothing"),
                                            GET_ATTR_FLOAT("width"),
                                            GET_ATTR_FLOAT("vmin"),
                                            bbox_points,
                                            p_noise_x,
                                            p_noise_y,
                                            nullptr, // stretching
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
