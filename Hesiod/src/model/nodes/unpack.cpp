/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Unpack::Unpack(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Unpack::Unpack");

  this->recompute_size_after_compute = true;

  // model
  this->node_caption = "Unpack";

  // inputs
  this->input_captions = {"vector"};
  this->input_types = {HeightMapVectorData().type()};

  // outputs (no output by default, will be updated at node compute
  // based on the number of components of the input vector)
  this->output_captions = {};
  this->output_types = {};

  // attributes -> none

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    // failsafe - generate an arbitrary fixed-size vector of outputs
    // to prevent issues at graph deserialization (since node update
    // order is not controlled with qt-node-editor)
    int default_size = 16;
    this->vector_out.clear();
    this->vector_out.reserve(default_size);

    for (int k = 0; k < default_size; k++)
      this->vector_out.push_back(std::make_shared<HeightMapData>(p_config));

    this->compute();
  }

  // documentation
  this->description = "Unpack node takes a list of heightmap as an input and distributes "
                      "them to its outlets.";

  this->input_descriptions = {};
  this->output_descriptions = {};
}

std::shared_ptr<QtNodes::NodeData> Unpack::outData(QtNodes::PortIndex port_index)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->vector_out[port_index]);
}

void Unpack::setInData(std::shared_ptr<QtNodes::NodeData> data,
                       QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapVectorData>(data);

  this->compute();
}

// --- computing

void Unpack::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  std::vector<hmap::HeightMap> *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
    if (p_in->size())
    {
      LOG->trace("size {}", p_in->size());

      Q_EMIT this->computingStarted();

      // copy every component if the input vector to an output
      this->vector_out.clear();
      this->vector_out.reserve(p_in->size());

      this->output_captions.clear();
      this->output_types.clear();

      for (size_t k = 0; k < p_in->size(); k++)
      {
        // FIX ME feels clumsy
        std::shared_ptr<HeightMapData> tmp = std::make_shared<HeightMapData>(
            this->p_config);
        *tmp->get_ref() = p_in->at(k);
        this->vector_out.push_back(tmp);

        this->output_captions.push_back("output");
        this->output_types.push_back(HeightMapData().type());
      }

      // propagate
      Q_EMIT this->computingFinished();
      this->trigger_outputs_updated();
    }
}

} // namespace hesiod
