/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
// #include <vector>

#include "highmap/heightmap.hpp"

#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Colorize::Colorize(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Colorize::Colorize");

  // model
  this->node_caption = "Colorize";

  // inputs
  this->input_captions = {"level", "alpha"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"texture"};
  this->output_types = {HeightMapRGBAData().type()};

  // attributes
  this->attr["colormap"] = NEW_ATTR_MAPENUM(get_colormap_mapping());
  this->attr["reverse_colormap"] = NEW_ATTR_BOOL(false);
  this->attr["reverse_alpha"] = NEW_ATTR_BOOL(false);
  this->attr["saturate_input"] = NEW_ATTR_BOOL(false);
  this->attr["saturate_input_range"] = NEW_ATTR_RANGE();
  this->attr["saturate_alpha"] = NEW_ATTR_BOOL(false);
  this->attr["saturate_alpha_range"] = NEW_ATTR_RANGE();
  this->attr_ordered_key = {"colormap",
                            "reverse_colormap",
                            "reverse_alpha",
                            "saturate_input",
                            "saturate_input_range",
                            "saturate_alpha",
                            "saturate_alpha_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapRGBAData>(p_config);
    this->compute();
  }
}

std::shared_ptr<QtNodes::NodeData> Colorize::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Colorize::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->level = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->alpha = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Colorize::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap     *p_level = HSD_GET_POINTER(this->level);
  hmap::HeightMap     *p_alpha = HSD_GET_POINTER(this->alpha);
  hmap::HeightMapRGBA *p_out = this->out->get_ref();

  // colorize
  if (p_level)
  {
    std::vector<std::vector<float>> colormap_colors = hesiod::get_colormap_data(
        GET_ATTR_MAPENUM("colormap"));

    // input saturation (clamping and then remapping to [0, 1])
    float cmin = 0.f;
    float cmax = 1.f;

    if (GET_ATTR_BOOL("saturate_input"))
    {
      float hmin = p_level->min();
      float hmax = p_level->max();

      hmap::Vec2<float> crange = GET_ATTR_RANGE("saturate_input_range");

      cmin = (1.f - crange.x) * hmin + crange.x * hmax;
      cmax = (1.f - crange.y) * hmin + crange.y * hmax;
    }

    // reverse alpha
    hmap::HeightMap  alpha_copy;
    hmap::HeightMap *p_alpha_copy = nullptr;

    if (!p_alpha)
      p_alpha_copy = p_alpha;
    else
    {
      alpha_copy = *p_alpha;
      p_alpha_copy = &alpha_copy;

      if (GET_ATTR_BOOL("reverse_alpha"))
        alpha_copy.inverse();

      if (GET_ATTR_BOOL("saturate_alpha"))
      {
        hmap::Vec2<float> arange = GET_ATTR_RANGE("saturate_alpha_range");
        hmap::transform(alpha_copy,
                        [&arange](hmap::Array &x)
                        { hmap::clamp(x, arange.x, arange.y); });
        alpha_copy.remap();
      }
    }

    // colorize
    p_out->colorize(*p_level,
                    cmin,
                    cmax,
                    colormap_colors,
                    p_alpha_copy,
                    GET_ATTR_BOOL("reverse_colormap"));
  }

  // propagate
  QtNodes::PortIndex const out_port_index = 0;

  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
