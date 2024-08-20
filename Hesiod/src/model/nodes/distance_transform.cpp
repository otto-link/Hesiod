/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

DistanceTransform::DistanceTransform(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("DistanceTransform::DistanceTransform");

  // model
  this->node_caption = "DistanceTransform";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["reverse"] = NEW_ATTR_BOOL(false);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"reverse", "_SEPARATOR_", "inverse", "remap"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "DistanceTransform is a distance map or distance image where each "
                      "pixel's value represents the shortest distance to the nearest "
                      "non-zero value in the input heightmap.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Output heightmap."};
}

std::shared_ptr<QtNodes::NodeData> DistanceTransform::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void DistanceTransform::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                  QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void DistanceTransform::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // not distributed, work on a single array
    hmap::Array z_array = p_in->to_array();

    if (GET_ATTR_BOOL("reverse"))
    {
      make_binary(z_array);
      z_array = 1.f - z_array;
    }

    z_array = hmap::distance_transform(z_array);

    p_out->from_array_interp(z_array);

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

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
