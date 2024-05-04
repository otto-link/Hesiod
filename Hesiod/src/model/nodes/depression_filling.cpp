/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/model/nodes/depression_filling.hpp"


namespace hesiod
{

DepressionFilling::DepressionFilling(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("DepressionFilling::DepressionFilling");

  // model
  this->node_caption = "DepressionFilling";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output", "fill map"};
  this->output_types = {HeightMapData().type(), HeightMapData().type()};

  // attributes
  this->attr["iterations"] = NEW_ATTR_INT(1000, 1, 5000);
  this->attr["epsilon"] = NEW_ATTR_FLOAT(1e-4, 1e-5, 1e-2, "%.5f");
  this->attr["remap fill map"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"iterations", "epsilon", "remap fill map"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->fill_map = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "DepressionFilling is used to fill depressions or sinks in an "
                      "heightmap. It ensures that there are no depressions, i.e. areas "
                      "within a digital elevation model that are surrounded by higher "
                      "terrain, with no outlet to lower areas.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Filled heightmap.", "Filling map."};

  this->attribute_descriptions["iterations"] = "Maximum number of iterations.";
  this->attribute_descriptions["epsilon"] = "Minimum slope tolerance.";
  this->attribute_descriptions["remap fill map"] = "Remap to [0, 1] the filling map.";
}

std::shared_ptr<QtNodes::NodeData> DepressionFilling::outData(
    QtNodes::PortIndex port_index)
{
  switch (port_index)
  {
  case 0:
    return std::static_pointer_cast<QtNodes::NodeData>(this->out);
  case 1:
  default:
    return std::static_pointer_cast<QtNodes::NodeData>(this->fill_map);
  }
}

void DepressionFilling::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                  QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void DepressionFilling::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();
  hmap::HeightMap *p_fill_map = this->fill_map->get_ref();

  if (p_in)
  {
    // work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();
    hmap::depression_filling(z_array,
                             GET_ATTR_INT("iterations"),
                             GET_ATTR_FLOAT("epsilon"));
    p_out->from_array_interp(z_array);

    hmap::transform(*p_fill_map,
                    *p_in,
                    *p_out,
                    [this](hmap::Array &out, hmap::Array &in1, hmap::Array &in2)
                    { out = in2 - in1; });

    if (GET_ATTR_BOOL("remap fill map"))
      p_fill_map->remap();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
