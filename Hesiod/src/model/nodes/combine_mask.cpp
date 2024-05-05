/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CombineMask::CombineMask(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("CombineMask::CombineMask");

  // model
  this->node_caption = "CombineMask";

  // inputs
  this->input_captions = {"input 1", "input 2"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["method"] = NEW_ATTR_MAPENUM(mask_combine_method_map, "intersection");

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "CombineMask performs basic logical operations on a pair of "
                      "heightmaps (assuming they are used as masks).";

  this->input_descriptions = {"Input mask.", "Input mask."};
  this->output_descriptions = {"Combined mask."};

  this->attribute_descriptions["method"] = "Combining method.";
}

std::shared_ptr<QtNodes::NodeData> CombineMask::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CombineMask::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in1 = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->in2 = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void CombineMask::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMap *p_in2 = HSD_GET_POINTER(this->in2);

  if (p_in1 && p_in2)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

    int method = GET_ATTR_MAPENUM("method");

    switch (method)
    {
    case MaskCombineMethod::UNION:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::maximum(a1, a2); };
      break;

    case MaskCombineMethod::INTERSECTION:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      { m = hmap::minimum(a1, a2); };
      break;

    case MaskCombineMethod::EXCLUSION:
      lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
      {
        m = a1 - a2;
        hmap::clamp_min(m, 0.f);
      };
      break;
    }

    hmap::transform(*p_out, *p_in1, *p_in2, lambda);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
