/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
// #include <vector>

#include "highmap/heightmap.hpp"

#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

MixTexture::MixTexture(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("MixTexture::MixTexture");

  // model
  this->node_caption = "MixTexture";

  // inputs
  this->input_captions = {"texture1", "texture2", "texture3", "texture4"};
  this->input_types = {HeightMapRGBAData().type(),
                       HeightMapRGBAData().type(),
                       HeightMapRGBAData().type(),
                       HeightMapRGBAData().type()};

  // outputs
  this->output_captions = {"texture"};
  this->output_types = {HeightMapRGBAData().type()};

  // attributes
  this->attr["use_sqrt_avg"] = NEW_ATTR_BOOL(true);
  this->attr["reset_output_alpha"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"use_sqrt_avg", "reset_output_alpha"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapRGBAData>(p_config);
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> MixTexture::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void MixTexture::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->in1 = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
    break;
  case 1:
    this->in2 = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
    break;
  case 2:
    this->in3 = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
    break;
  case 3:
    this->in4 = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
  }

  this->compute();
}

// --- computing

void MixTexture::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMapRGBA *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::HeightMapRGBA *p_in2 = HSD_GET_POINTER(this->in2);
  hmap::HeightMapRGBA *p_in3 = HSD_GET_POINTER(this->in3);
  hmap::HeightMapRGBA *p_in4 = HSD_GET_POINTER(this->in4);
  hmap::HeightMapRGBA *p_out = this->out->get_ref();

  std::vector<hmap::HeightMapRGBA *> ptr_list = {};
  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if (ptr_list.size())
  {
    *p_out = mix_heightmap_rgba(ptr_list, GET_ATTR_BOOL("use_sqrt_avg"));

    if (GET_ATTR_BOOL("reset_output_alpha"))
      p_out->set_alpha(1.f);
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
