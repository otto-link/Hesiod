/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Remap::Remap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Remap::Remap");

  // model
  this->node_caption = "Remap";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["remap"] = NEW_ATTR_RANGE();

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The Remap operator is used to adjust the elevation values across "
                      "the entire dataset, effectively changing the range of elevation "
                      "data to match a desired output scale or to normalize the data.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Remapped heightmap."};

  this->attribute_descriptions
      ["remap"] = "Target range: define the new minimum and maximum values to remap the "
                  "elevation values to.";
}

std::shared_ptr<QtNodes::NodeData> Remap::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Remap::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Remap::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    p_out->remap(GET_ATTR_RANGE("remap").x, GET_ATTR_RANGE("remap").y);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
