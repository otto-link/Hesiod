/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/constants/cmap.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_colorize_cmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "level");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "alpha");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "noise");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG(node));

  // attribute(s)
  node.add_attr<EnumAttribute>(
      "colormap",
      "colormap",
      hesiod::CmapManager::get_instance().get_colormap_name_mapping());
  node.add_attr<BoolAttribute>("reverse_colormap", "reverse_colormap", false);
  node.add_attr<BoolAttribute>("reverse_alpha", "reverse_alpha", false);
  node.add_attr<BoolAttribute>("clamp_alpha", "clamp_alpha", true);
  node.add_attr<RangeAttribute>("saturate_input", "saturate_input", false);
  node.add_attr<RangeAttribute>("saturate_alpha", "saturate_alpha", false);

  // attribute(s) order
  node.set_attr_ordered_key({"colormap",
                             "reverse_colormap",
                             "reverse_alpha",
                             "clamp_alpha",
                             "_SEPARATOR_",
                             "saturate_input",
                             "saturate_alpha"});
}

void compute_colorize_cmap_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_level = node.get_value_ref<hmap::VirtualArray>("level");

  // // colorize
  // if (p_level)
  // {
  //   hmap::VirtualArray  *p_alpha = node.get_value_ref<hmap::VirtualArray>("alpha");
  //   hmap::VirtualArray  *p_noise = node.get_value_ref<hmap::VirtualArray>("noise");
  //   hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  //   std::vector<std::vector<float>>
  //       colormap_colors = hesiod::CmapManager::get_instance().get_colormap_data(
  //           node.get_attr<EnumAttribute>("colormap"));

  //   // input saturation (clamping and then remapping to [0, 1])
  //   float cmin = 0.f;
  //   float cmax = 1.f;

  //   if (node.get_attr_ref<RangeAttribute>("saturate_input")->get_is_active())
  //   {
  //     float hmin = p_level->min(node.cfg().cm_cpu);
  //     float hmax = p_level->max(node.cfg().cm_cpu);

  //     hmap::Vec2<float> crange = node.get_attr<RangeAttribute>("saturate_input");

  //     cmin = (1.f - crange.x) * hmin + crange.x * hmax;
  //     cmax = (1.f - crange.y) * hmin + crange.y * hmax;
  //   }

  //   if (p_noise)
  //   {
  //     cmin += p_noise->min(node.cfg().cm_cpu);
  //     cmax += p_noise->max(node.cfg().cm_cpu);
  //   }

  //   // reverse alpha
  //   hmap::VirtualArray  alpha_copy;
  //   hmap::VirtualArray *p_alpha_copy = nullptr;

  //   if (!p_alpha)
  //     p_alpha_copy = p_alpha;
  //   else
  //   {
  //     alpha_copy = *p_alpha;
  //     p_alpha_copy = &alpha_copy;

  //     if (node.get_attr<BoolAttribute>("clamp_alpha"))
  //       hmap::for_each_tile(alpha_copy, [](hmap::Array &x) { hmap::clamp(x, 0.f, 1.f);
  //       });

  //     if (node.get_attr<BoolAttribute>("reverse_alpha"))
  //       alpha_copy.inverse();

  //     if (node.get_attr_ref<RangeAttribute>("saturate_alpha")->get_is_active())
  //     {
  //       hmap::Vec2<float> arange = node.get_attr<RangeAttribute>("saturate_alpha");
  //       hmap::for_each_tile(alpha_copy,
  //                           [&arange](hmap::Array &x)
  //                           { hmap::clamp(x, arange.x, arange.y); });
  //       alpha_copy.remap();
  //     }
  //   }

  //   // colorize
  //   p_out->colorize(*p_level,
  //                   cmin,
  //                   cmax,
  //                   colormap_colors,
  //                   p_alpha_copy,
  //                   node.get_attr<BoolAttribute>("reverse_colormap"),
  //                   p_noise);
  // }
}

} // namespace hesiod
