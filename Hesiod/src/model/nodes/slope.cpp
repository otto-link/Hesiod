/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Slope::Slope(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Slope::Slope");

  // model
  this->node_caption = "Slope";

  // inputs
  this->input_captions = {"dr"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["talus_global"] = NEW_ATTR_FLOAT(2.f, 0.01f, 32.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"angle",
                            "talus_global",
                            "center.x",
                            "center.y",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "Slope is function used to represent continuous terrain slope.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (normal direction)."};
  this->output_descriptions = {"Slope heightmap."};

  this->attribute_descriptions["angle"] = "Angle.";
  this->attribute_descriptions["talus_global"] = "Slope slope...";
  this->attribute_descriptions["center.x"] = "Center x coordinate.";
  this->attribute_descriptions["center.y"] = "Center y coordinate.";
}

std::shared_ptr<QtNodes::NodeData> Slope::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Slope::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex /* port_index */)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  this->dr = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Slope::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  // base noise function
  hmap::HeightMap *p_dr = HSD_GET_POINTER(this->dr);
  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  hmap::fill(
      *p_out,
      p_dr,
      [this, &center](hmap::Vec2<int> shape, hmap::Vec4<float> bbox, hmap::Array *p_noise)
      {
        return hmap::slope(shape,
                           GET_ATTR_FLOAT("angle"),
                           GET_ATTR_FLOAT("talus_global"),
                           p_noise,
                           nullptr,
                           nullptr,
                           center,
                           bbox);
      });

  // post-process
  post_process_heightmap(*p_out,
                         GET_ATTR_BOOL("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR_BOOL("remap"),
                         GET_ATTR_RANGE("remap_range"));

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
