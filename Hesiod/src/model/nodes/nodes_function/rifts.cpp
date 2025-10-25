/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_rifts_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  std::vector<float> kw_default = {4.f, 1.2f};
  ADD_ATTR(WaveNbAttribute, "kw", kw_default, 0.f, 32.f, false);
  ADD_ATTR(FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(FloatAttribute, "amplitude", 0.1f, 0.f, 1.f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "k_smooth_bottom", 0.05f, 0.f, 0.3f);
  ADD_ATTR(FloatAttribute, "k_smooth_top", 0.05f, 0.f, 0.3f);
  ADD_ATTR(FloatAttribute, "radial_spread_amp", 0.2f, -1.f, 1.f);
  ADD_ATTR(FloatAttribute, "elevation_noise_amp", 0.1f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "clamp_vmin", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "remap_vmin", 0.6f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "elevation_noise_shift", 0.f, -1.f, 1.f);
  ADD_ATTR(BoolAttribute, "apply_mask", true);
  ADD_ATTR(BoolAttribute, "reverse_mask", true);
  ADD_ATTR(FloatAttribute, "mask_gamma", 1.f, 0.01f, 4.f);
  ADD_ATTR(Vec2FloatAttribute, "center");

  // attribute(s) order
  p_node->set_attr_ordered_key({"_TEXT_Base paramaters",
                                "kw",
                                "angle",
                                "amplitude",
                                "seed",
                                "_TEXT_Edge smoothing",
                                "k_smooth_bottom",
                                "k_smooth_top",
                                "_TEXT_Ridge geometry",
                                "radial_spread_amp",
                                "elevation_noise_amp",
                                "clamp_vmin",
                                "remap_vmin",
                                "elevation_noise_shift",
                                "center",
                                "_TEXT_Masking",
                                "apply_mask",
                                "reverse_mask",
                                "mask_gamma"});

  setup_pre_process_mask_attributes(p_node);
  setup_post_process_heightmap_attributes(p_node, true);

  add_wip_warning_label(p_node);
}

void compute_rifts_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(p_node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    // remap to [0, 1] as required by this filter
    float hmin = p_out->min();
    float hmax = p_out->max();
    p_out->remap(0.f, 1.f, hmin, hmax);

    hmap::transform(
        {p_out, p_dx, p_dy, p_mask},
        [p_node](std::vector<hmap::Array *> p_arrays,
                 hmap::Vec2<int> /* shape */,
                 hmap::Vec4<float> bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_dx = p_arrays[1];
          hmap::Array *pa_dy = p_arrays[2];
          hmap::Array *pa_mask = p_arrays[3];

          hmap::gpu::rifts(*pa_out,
                           GET("kw", WaveNbAttribute),
                           GET("angle", FloatAttribute),
                           GET("amplitude", FloatAttribute),
                           GET("seed", SeedAttribute),
                           GET("elevation_noise_shift", FloatAttribute),
                           GET("k_smooth_bottom", FloatAttribute),
                           GET("k_smooth_top", FloatAttribute),
                           GET("radial_spread_amp", FloatAttribute),
                           GET("elevation_noise_amp", FloatAttribute),
                           GET("clamp_vmin", FloatAttribute),
                           GET("remap_vmin", FloatAttribute),
                           GET("apply_mask", BoolAttribute),
                           GET("reverse_mask", BoolAttribute),
                           GET("mask_gamma", FloatAttribute),
                           pa_dx,
                           pa_dy,
                           pa_mask,
                           GET("center", Vec2FloatAttribute),
                           bbox);
        },
        ctx.app_settings.node_editor.hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // remap to original range
    p_out->remap(hmin, hmax, 0.f, 1.f);

    // post-process
    post_process_heightmap(p_node, *p_out, p_in);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
