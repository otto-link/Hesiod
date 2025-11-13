/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_strata_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  std::vector<float> kw_default;

  ADD_ATTR(FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(FloatAttribute, "slope", 2.f, 0.01f, 10.f);
  ADD_ATTR(FloatAttribute, "kz", 1.f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "gamma", 0.5f, 0.01f, 2.f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(BoolAttribute, "linear_gamma", true);
  ADD_ATTR(IntAttribute, "octaves", 6, 0, 32);
  ADD_ATTR(FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);
  ADD_ATTR(FloatAttribute, "gamma_noise_ratio", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "noise_amp", 0.4f, 0.f, 1.f);

  kw_default = {4.f, 4.f};
  ADD_ATTR(WaveNbAttribute, "noise_kw", kw_default, 0.f, 32.f, true);

  kw_default = {4.f, 1.5f};
  ADD_ATTR(WaveNbAttribute, "ridge_noise_kw", kw_default, 0.f, 32.f, false);

  ADD_ATTR(FloatAttribute, "ridge_angle_shift", 45.f, -180.f, 180.f);
  ADD_ATTR(FloatAttribute, "ridge_noise_amp", 0.4f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "ridge_clamp_vmin", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "ridge_remap_vmin", 0.6f, 0.f, 1.f);
  ADD_ATTR(BoolAttribute, "apply_elevation_mask", true);
  ADD_ATTR(BoolAttribute, "apply_ridge_mask", true);
  ADD_ATTR(FloatAttribute, "mask_gamma", 1.f, 0.01f, 4.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"_TEXT_Base paramaters",
                                "angle",
                                "slope",
                                "gamma",
                                "kz",
                                "seed",
                                "linear_gamma",
                                "_TEXT_Fractal strata",
                                "octaves",
                                "lacunarity",
                                "gamma_noise_ratio",
                                "_TEXT_Background noise",
                                "noise_amp",
                                "noise_kw",
                                "_TEXT_Ridges",
                                "ridge_noise_kw",
                                "ridge_angle_shift",
                                "ridge_noise_amp",
                                "ridge_clamp_vmin",
                                "ridge_remap_vmin",
                                "_TEXT_Masking",
                                "apply_elevation_mask",
                                "apply_ridge_mask",
                                "mask_gamma"});

  setup_pre_process_mask_attributes(p_node);
  setup_post_process_heightmap_attributes(p_node, true);

  add_wip_warning_label(p_node);
}

void compute_strata_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
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
        {p_out, p_mask},
        [p_node](std::vector<hmap::Array *> p_arrays,
                 hmap::Vec2<int> /* shape */,
                 hmap::Vec4<float> bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];

          hmap::gpu::strata(*pa_out,
                            GET("angle", FloatAttribute),
                            GET("slope", FloatAttribute),
                            GET("gamma", FloatAttribute),
                            GET("seed", SeedAttribute),
                            GET("linear_gamma", BoolAttribute),
                            GET("kz", FloatAttribute),
                            GET("octaves", IntAttribute),
                            GET("lacunarity", FloatAttribute),
                            GET("gamma_noise_ratio", FloatAttribute),
                            GET("noise_amp", FloatAttribute),
                            GET("noise_kw", WaveNbAttribute),
                            GET("ridge_noise_kw", WaveNbAttribute),
                            GET("ridge_angle_shift", FloatAttribute),
                            GET("ridge_noise_amp", FloatAttribute),
                            GET("ridge_clamp_vmin", FloatAttribute),
                            GET("ridge_remap_vmin", FloatAttribute),
                            GET("apply_elevation_mask", BoolAttribute),
                            GET("apply_ridge_mask", BoolAttribute),
                            GET("mask_gamma", FloatAttribute),
                            pa_mask,
                            bbox);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // remap to original range
    p_out->remap(hmin, hmax, 0.f, 1.f);

    // post-process
    post_process_heightmap(p_node, *p_out, p_in);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
