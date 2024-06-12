/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CloudRandom::CloudRandom(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("CloudRandom::CloudRandom");

  // model
  this->node_caption = "CloudRandom";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};

  // attributes
  this->attr["npoints"] = NEW_ATTR_INT(50, 1, 1000);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"npoints", "seed", "_SEPARATOR_", "remap", "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<CloudData>();
    this->compute();
  }

  // documentation
  this->description = "Random set of points.";

  this->input_descriptions = {};
  this->output_descriptions = {"Set of points (x, y) and elevations z."};

  this->attribute_descriptions["npoints"] = "Number of points.";
  this->attribute_descriptions["seed"] = "Random seed number.";
}

std::shared_ptr<QtNodes::NodeData> CloudRandom::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CloudRandom::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                            QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void CloudRandom::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  *this->out->get_ref() = hmap::Cloud(GET_ATTR_INT("npoints"), GET_ATTR_SEED("seed"));

  if (GET_ATTR_BOOL("remap"))
    this->out->get_ref()->remap_values(GET_ATTR_RANGE("remap_range").x,
                                       GET_ATTR_RANGE("remap_range").y);

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
