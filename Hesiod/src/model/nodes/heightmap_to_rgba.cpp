/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

HeightmapToRGBA::HeightmapToRGBA(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("HeightmapToRGBA::HeightmapToRGBA");

  // model
  this->node_caption = "HeightmapToRGBA";

  // inputs
  this->input_captions = {"R", "G", "B", "A"};
  this->input_types = {HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type(),
                       HeightMapData().type()};

  // outputs
  this->output_captions = {"RGBA"};
  this->output_types = {HeightMapRGBAData().type()};

  // attributes
  this->attr_ordered_key = {};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapRGBAData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "HeightmapToRGBA converts a series of heightmaps into an RGBA "
                      "splatmap.";

  this->input_descriptions = {"Red channel.",
                              "Green channel.",
                              "Blue channel.",
                              "Alpha channel."};
  this->output_descriptions = {"RGBA heightmap."};
}

std::shared_ptr<QtNodes::NodeData> HeightmapToRGBA::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void HeightmapToRGBA::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->r = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->g = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 2:
    this->b = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 3:
    this->a = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void HeightmapToRGBA::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_r = HSD_GET_POINTER(this->r);
  hmap::HeightMap *p_g = HSD_GET_POINTER(this->g);
  hmap::HeightMap *p_b = HSD_GET_POINTER(this->b);
  hmap::HeightMap *p_a = HSD_GET_POINTER(this->a);

  if (p_r || p_g || p_b || p_a)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMapRGBA *p_out = this->out->get_ref();

    hmap::Vec2<int> shape;
    hmap::Vec2<int> tiling;
    float           overlap = 0.f;

    for (auto ptr : {p_r, p_g, p_b, p_a})
      if (ptr)
      {
        shape = ptr->shape;
        tiling = ptr->tiling;
        overlap = ptr->overlap;
        break;
      }

    hmap::HeightMap hr = p_r == nullptr ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                                        : *p_r;
    hmap::HeightMap hg = p_g == nullptr ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                                        : *p_g;
    hmap::HeightMap hb = p_b == nullptr ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                                        : *p_b;
    hmap::HeightMap ha = p_a == nullptr ? hmap::HeightMap(shape, tiling, overlap, 1.f)
                                        : *p_a;

    *p_out = hmap::HeightMapRGBA(hr, hg, hb, ha);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
