/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/colorize.hpp"
#include "highmap/virtual_array/virtual_texture.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_color_adjust_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualTexture>(gnode::PortType::IN, "texture in");
  node.add_port<hmap::VirtualTexture>(gnode::PortType::OUT,
                                      "texture out",
                                      CONFIG_TEX(node));

  // attribute(s)
  node.add_attr<RangeAttribute>("levels", "Levels");
  node.add_attr<FloatAttribute>("exposure", "Exposure", 0.f, -10.f, 10.f);
  node.add_attr<FloatAttribute>("contrast", "Contrast", 1.f, 0.f, 4.f);
  node.add_attr<FloatAttribute>("saturation", "Saturation", 1.f, 0.f, 4.f);
  node.add_attr<FloatAttribute>("temperature", "Temperature", 0.f, -1.f, 1.f);
  node.add_attr<FloatAttribute>("gamma", "Gamma Correction", 1.f, 0.1f, 4.f);
  node.add_attr<FloatAttribute>("dither_amp", "Dither Amplitude", 0.f, 0.f, 0.5f);
  node.add_attr<BoolAttribute>("filmic_tonemap", "Filmic Tonemap", false);
  node.add_attr<BoolAttribute>("aces_tonemap", "ACES Tonemap", false);

  // struct ColorAdjust
  // {
  //   float in_min = 0.0f;
  //   float in_max = 1.0f;
  //   float exposure = 0.0f;
  //   float contrast = 1.0f;
  //   float saturation = 1.0f;
  //   float temperature = 0.0f;
  //   float gamma = 2.2f;
  //   float dither_amp = 1.f / 255.f;
  //   bool  filmic_tonemap = false;
  //   bool  aces_tonemap = false;
  // };

  // attribute(s) order
  node.set_attr_ordered_key({"levels",
                             "exposure",
                             "contrast",
                             "saturation",
                             "temperature",
                             "gamma",
                             "dither_amp",
                             "filmic_tonemap",
                             "aces_tonemap"});
}

void compute_color_adjust_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualTexture *p_in = node.get_value_ref<hmap::VirtualTexture>("texture in");

  if (p_in)
  {
    hmap::VirtualTexture *p_out = node.get_value_ref<hmap::VirtualTexture>("texture out");
    p_out->copy_from(*p_in, node.cfg().cm_cpu);

    hmap::ColorAdjust param = {
        .in_min = node.get_attr<RangeAttribute>("levels")[0],
        .in_max = node.get_attr<RangeAttribute>("levels")[1],
        .exposure = node.get_attr<FloatAttribute>("exposure"),
        .contrast = node.get_attr<FloatAttribute>("contrast"),
        .saturation = node.get_attr<FloatAttribute>("saturation"),
        .temperature = node.get_attr<FloatAttribute>("temperature"),
        .gamma = node.get_attr<FloatAttribute>("gamma"),
        .dither_amp = node.get_attr<FloatAttribute>("dither_amp"),
        .filmic_tonemap = node.get_attr<BoolAttribute>("filmic_tonemap"),
        .aces_tonemap = node.get_attr<BoolAttribute>("aces_tonemap")};

    hmap::color_adjust(*p_out, param, node.cfg().cm_cpu);
  }
}

} // namespace hesiod
