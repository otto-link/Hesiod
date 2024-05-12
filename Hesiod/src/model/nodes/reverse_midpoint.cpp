/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ReverseMidpoint::ReverseMidpoint(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ReverseMidpoint::ReverseMidpoint");

  // model
  this->node_caption = "ReverseMidpoint";

  // inputs
  this->input_captions = {"path"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"heightmap"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["noise_scale"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"noise_scale", "seed", "_SEPARATOR_", "inverse", "remap"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ReverseMidpoint generates an heightmap. It uses a polyline as a "
                      "base, then recursively interpolate and displace midpoints to "
                      "generate a terrain.";

  this->input_descriptions = {"Set of points (x, y) and elevations z."};
  this->output_descriptions = {"Interpolated heightmap."};

  this->attribute_descriptions["noise_scale"] = "Added noise scaling.";
  this->attribute_descriptions["seed"] = "Random seed number.";
}

std::shared_ptr<QtNodes::NodeData> ReverseMidpoint::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ReverseMidpoint::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void ReverseMidpoint::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::Path *p_path = HSD_GET_POINTER(this->in);

  if (p_path)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    if (p_path->get_npoints() > 1)
    {
      hmap::Array       path_array = hmap::Array(p_out->shape);
      hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
      p_path->to_array(path_array, bbox);

      hmap::Array z = hmap::reverse_midpoint(path_array,
                                             GET_ATTR_SEED("seed"),
                                             GET_ATTR_FLOAT("noise_scale"),
                                             0.f); // threshold

      p_out->from_array_interp_nearest(z);

      // post-process
      post_process_heightmap(*p_out,
                             GET_ATTR_BOOL("inverse"),
                             false, // smooth
                             0,
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             GET_ATTR_BOOL("remap"),
                             {0.f, 1.f});
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
