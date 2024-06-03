/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

MakePeriodicStitching::MakePeriodicStitching(const ModelConfig *p_config)
    : BaseNode(p_config)
{
  LOG->trace("MakePeriodicStitching::MakePeriodicStitching");

  // model
  this->node_caption = "MakePeriodicStitching";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["overlap"] = NEW_ATTR_FLOAT(0.4f, 0.05f, 0.95f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "MakePeriodicStitching makes the input heightmap tileable by "
                      "seamlessly stitching together the domain boundaries.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Binary heightmap."};

  this->attribute_descriptions["overlap"] = "Overlap ratio at the boundaries.";
}

std::shared_ptr<QtNodes::NodeData> MakePeriodicStitching::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void MakePeriodicStitching::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                      QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void MakePeriodicStitching::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    hmap::Array in_array = p_in->to_array();
    hmap::Array out_array = hmap::Array(p_out->shape);

    out_array = hmap::make_periodic_stitching(in_array, GET_ATTR_FLOAT("overlap"));

    p_out->from_array_interp_nearest(out_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
