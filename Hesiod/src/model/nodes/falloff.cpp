/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Falloff::Falloff(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Falloff::Falloff");

  // model
  this->node_caption = "Falloff";

  // inputs
  this->input_captions = {"input", "dr"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["strength"] = NEW_ATTR_FLOAT(1.f, 0.f, 4.f, "%.2f");
  this->attr["distance_function"] = NEW_ATTR_MAPENUM(distance_function_map, "Euclidian");

  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"strength", "distance_function", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Falloff is an operator that enforces values close to zero at "
                      "the domain edges.";

  this->input_descriptions = {
      "Input heightmap.",
      "Displacement with respect to the domain size (radial direction)."};
  this->output_descriptions = {"Filtered heightmap."};

  this->attribute_descriptions["strength"] = "Falloff strength.";
  this->attribute_descriptions
      ["distance_function"] = "Measure used for the distance calculation.";
}

std::shared_ptr<QtNodes::NodeData> Falloff::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Falloff::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->dr = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Falloff::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_dr = HSD_GET_POINTER(this->dr);
    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    float strength = GET_ATTR_FLOAT("strength");

    if (!p_dr)
      hmap::transform(*p_out,
                      [this, &strength](hmap::Array &z, hmap::Vec4<float> bbox)
                      {
                        hmap::falloff(
                            z,
                            strength,
                            (hmap::DistanceFunction)GET_ATTR_MAPENUM("distance_function"),
                            nullptr,
                            bbox);
                      });

    else
      hmap::transform(
          *p_out,
          *p_dr,
          [this, &strength](hmap::Array &z, hmap::Array &dr, hmap::Vec4<float> bbox)
          {
            hmap::falloff(z,
                          strength,
                          (hmap::DistanceFunction)GET_ATTR_MAPENUM("distance_function"),
                          &dr,
                          bbox);
          });

    if (GET_ATTR_BOOL("remap"))
      p_out->remap(GET_ATTR_RANGE("remap_range").x, GET_ATTR_RANGE("remap_range").y);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
