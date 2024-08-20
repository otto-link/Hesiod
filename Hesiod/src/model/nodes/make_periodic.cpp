/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

MakePeriodic::MakePeriodic(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("MakePeriodic::MakePeriodic");

  // model
  this->node_caption = "MakePeriodic";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["overlap"] = NEW_ATTR_FLOAT(0.25f, 0.05f, 0.5f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "MakePeriodic makes the input heightmap tileable by building a "
                      "smooth lineat transition at the boundaries.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Binary heightmap."};

  this->attribute_descriptions["overlap"] = "Overlap ratio at the boundaries.";
}

std::shared_ptr<QtNodes::NodeData> MakePeriodic::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void MakePeriodic::setInData(std::shared_ptr<QtNodes::NodeData> data,
                             QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void MakePeriodic::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    int nbuffer = std::max(1, (int)(GET_ATTR_FLOAT("overlap") * p_out->shape.x));

    hmap::Array in_array = p_in->to_array();
    hmap::Array out_array = in_array;

    hmap::make_periodic(out_array, nbuffer);

    p_out->from_array_interp_nearest(out_array);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
