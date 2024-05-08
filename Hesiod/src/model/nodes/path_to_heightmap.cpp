/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathToHeightmap::PathToHeightmap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("PathToHeightmap::PathToHeightmap");

  // model
  this->node_caption = "PathToHeightmap";

  // inputs
  this->input_captions = {"path"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"heightmap"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["filled"] = NEW_ATTR_BOOL(false);

  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");

  this->attr_ordered_key =
      {"filled", "_SEPARATOR_", "remap", "inverse", "smoothing", "smoothing_radius"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "PathToHeightmap Project path points to an heightmap.";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions["filled"] = "Whether the resulting path contour is filled "
                                           "(input Path needs to be closed).";
}

std::shared_ptr<QtNodes::NodeData> PathToHeightmap::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathToHeightmap::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void PathToHeightmap::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::Path *p_path = HSD_GET_POINTER(this->in);

  if (p_path)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    if (p_path->get_npoints() > 1)
    {
      if (!GET_ATTR_BOOL("filled"))
      {
        hmap::fill(*p_out,
                   [p_path](hmap::Vec2<int> shape, hmap::Vec4<float> bbox)
                   {
                     hmap::Array z = hmap::Array(shape);
                     p_path->to_array(z, bbox);
                     return z;
                   });
      }
      else
      {
        // work on a single array as a temporary solution
        hmap::Array       z_array = hmap::Array(p_out->shape);
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);

        p_path->to_array(z_array, bbox, true);
        p_out->from_array_interp_nearest(z_array);
      }

      // post-process
      post_process_heightmap(*p_out,
                             GET_ATTR_BOOL("inverse"),
                             GET_ATTR_BOOL("smoothing"),
                             GET_ATTR_FLOAT("smoothing_radius"),
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
