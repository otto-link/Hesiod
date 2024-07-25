/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Paraboloid::Paraboloid(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Paraboloid::Paraboloid");

  // model
  this->node_caption = "Paraboloid";

  // inputs
  this->input_captions = {"dx", "dy", "envelope"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["a"] = NEW_ATTR_FLOAT(1.f, 0.01f, 5.f);
  this->attr["b"] = NEW_ATTR_FLOAT(1.f, 0.01f, 5.f);
  this->attr["v0"] = NEW_ATTR_FLOAT(0.f, -2.f, 2.f);
  this->attr["reverse_x"] = NEW_ATTR_BOOL(false);
  this->attr["reverse_y"] = NEW_ATTR_BOOL(false);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"angle",
                            "a",
                            "b",
                            "v0",
                            "reverse_x",
                            "reverse_y",
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
  this->description = ".";

  this->input_descriptions = {
      "Displacement with respect to the domain size (x-direction).",
      "Displacement with respect to the domain size (y-direction).",
      "Output noise amplitude envelope."};
  this->output_descriptions = {"Paraboloid."};

  this->attribute_descriptions["angle"] = "Angle.";
  this->attribute_descriptions["a"] = "Curvature parameter, first principal axis.";
  this->attribute_descriptions["b"] = "Curvature parameter, second principal axis.";
  this->attribute_descriptions["v0"] = "Value at the paraboloid center.";
  this->attribute_descriptions
      ["reverse_x"] = "Reverse coefficient of first principal axis.";
  this->attribute_descriptions
      ["reverse_y"] = "Reverse coefficient of second principal axis.";
  this->attribute_descriptions["center.x"] = "Center x coordinate.";
  this->attribute_descriptions["center.y"] = "Center y coordinate.";
}

std::shared_ptr<QtNodes::NodeData> Paraboloid::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Paraboloid::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->dx = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->dy = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->envelope = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Paraboloid::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
  hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);
  hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);
  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::Vec2<float> center;
  center.x = GET_ATTR_FLOAT("center.x");
  center.y = GET_ATTR_FLOAT("center.y");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             [this, &center](hmap::Vec2<int>   shape,
                             hmap::Vec4<float> bbox,
                             hmap::Array      *p_noise_x,
                             hmap::Array      *p_noise_y)
             {
               return hmap::paraboloid(shape,
                                       GET_ATTR_FLOAT("angle"),
                                       GET_ATTR_FLOAT("a"),
                                       GET_ATTR_FLOAT("b"),
                                       GET_ATTR_FLOAT("v0"),
                                       GET_ATTR_BOOL("reverse_x"),
                                       GET_ATTR_BOOL("reverse_y"),
                                       p_noise_x,
                                       p_noise_y,
                                       nullptr,
                                       center,
                                       bbox);
             });

  // add envelope
  if (p_env)
  {
    float hmin = p_out->min();
    hmap::transform(*p_out,
                    *p_env,
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

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
