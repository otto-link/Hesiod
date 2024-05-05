/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
// #include <vector>

#include "highmap/heightmap.hpp"

#include "hesiod/model/cmap.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ColorizeGradient::ColorizeGradient(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("ColorizeGradient::ColorizeGradient");

  // model
  this->node_caption = "ColorizeGradient";

  // inputs
  this->input_captions = {"level", "alpha"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"texture"};
  this->output_types = {HeightMapRGBAData().type()};

  // attributes
  std::vector<std::vector<float>> default_gradient = {{0.f, 0.f, 0.f, 0.f, 1.f},
                                                      {1.f, 0.f, 1.f, 0.f, 1.f}};

  this->attr["gradient"] = NEW_ATTR_COLORGRADIENT(default_gradient);
  this->attr["reverse_colormap"] = NEW_ATTR_BOOL(false);
  this->attr["reverse_alpha"] = NEW_ATTR_BOOL(false);
  this->attr["clamp_alpha"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"gradient",
                            "reverse_colormap",
                            "reverse_alpha",
                            "clamp_alpha"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapRGBAData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ColorizeGradient generates a texture based on colormaps to assign "
                      "colors to data values.";

  this->input_descriptions = {"Data values for color selection.", "Texture alpha map."};
  this->output_descriptions = {"Texture (RGBA)."};

  this->attribute_descriptions
      ["gradient"] = "Colormap as a manually defined color gradient.";
  this->attribute_descriptions["reverse_colormap"] = "Reverse the colormap range.";
  this->attribute_descriptions["reverse_alpha"] = "Reverse the input alpha map.";
  this->attribute_descriptions["clamp_alpha"] = "Clamp to [0, 1] to input alpha map.";
}

std::shared_ptr<QtNodes::NodeData> ColorizeGradient::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ColorizeGradient::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void ColorizeGradient::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_level = HSD_GET_POINTER(this->level);

  if (p_level)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap     *p_alpha = HSD_GET_POINTER(this->alpha);
    hmap::HeightMapRGBA *p_out = this->out->get_ref();

    // define colormap based on color gradient
    std::vector<std::vector<float>> gradient = GET_ATTR_COLORGRADIENT("gradient");

    std::vector<std::vector<float>> colormap_colors = {};

    for (auto &data : gradient)
    {
      // TODO add position interpolation

      colormap_colors.push_back({data[1], data[2], data[3]});
      // LOG_DEBUG("%f %f %f",
      //           colormap_colors.back()[0],
      //           colormap_colors.back()[1],
      //           colormap_colors.back()[2]);
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

      if (GET_ATTR_BOOL("clamp_alpha"))
        hmap::transform(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f); });

      if (GET_ATTR_BOOL("reverse_alpha"))
        alpha_copy.inverse();
    }

    // colorize
    float cmin = p_level->min();
    float cmax = p_level->max();

    p_out->colorize(*p_level,
                    cmin,
                    cmax,
                    colormap_colors,
                    p_alpha_copy,
                    GET_ATTR_BOOL("reverse_colormap"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
