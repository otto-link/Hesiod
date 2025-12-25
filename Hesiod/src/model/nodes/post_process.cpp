/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void post_apply_enveloppe(BaseNode &node, hmap::Heightmap &h, hmap::Heightmap *p_env)
{
  Logger::log()->trace("post_apply_enveloppe: [{}]/[{}]",
                       node.get_node_type(),
                       node.get_id());

  if (p_env)
  {
    float hmin = h.min();

    hmap::transform(
        {&h, p_env},
        [hmin](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_env = p_arrays[1];

          *pa_out -= hmin;
          *pa_out *= *pa_env;
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }
}

void post_process_heightmap(BaseNode         &node,
                            hmap::Heightmap  &h,
                            bool              inverse,
                            bool              smoothing,
                            float             smoothing_radius,
                            bool              saturate,
                            hmap::Vec2<float> saturate_range,
                            float             saturate_k,
                            bool              remap,
                            hmap::Vec2<float> remap_range)
{
  if (inverse)
    h.inverse();

  if (smoothing)
  {
    int ir = std::max(1, (int)(smoothing_radius * h.shape.x));

    hmap::transform(
        {&h},
        [&ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          return hmap::gpu::smooth_cpulse(*pa_out, ir);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    h.smooth_overlap_buffers();
  }

  if (saturate)
  {
    float hmin = h.min();
    float hmax = h.max();

    // node parameters are assumed normalized and thus in [0, 1],
    // they need to be rescaled
    float smin_n = hmin + saturate_range.x * (hmax - hmin);
    float smax_n = hmax - (1.f - saturate_range.y) * (hmax - hmin);
    float k_n = std::max(1e-6f, saturate_k * (hmax - hmin));

    hmap::transform(h,
                    [&smin_n, &smax_n, &k_n](hmap::Array &array)
                    { hmap::clamp_smooth(array, smin_n, smax_n, k_n); });

    // keep original amplitude
    h.remap(hmin, hmax);
  }

  if (remap)
    h.remap(remap_range.x, remap_range.y);
}

void post_process_heightmap(BaseNode &node, hmap::Heightmap &h, hmap::Heightmap *p_in)
{
  Logger::log()->trace("post_process_heightmap: [{}]/[{}]",
                       node.get_node_type(),
                       node.get_id());

  // mix
  if (p_in)
  {
    // mix
    float k = 0.1f; // TODO hardcoded?
    int   ir = 0;
    int   method = node.get_attr<EnumAttribute>("post_mix_method");
    blend_heightmaps(h, *p_in, h, static_cast<BlendingMethod>(method), k, ir);

    // lerp between input and output
    float t = node.get_attr<FloatAttribute>("post_mix");

    hmap::transform(
        {&h, p_in},
        [t](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::lerp(*pa_in, *pa_out, t);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }

  // inverse
  if (node.get_attr<BoolAttribute>("post_inverse"))
    h.inverse();

  // gamma
  float post_gamma = node.get_attr<FloatAttribute>("post_gamma");

  if (post_gamma != 1.f)
  {
    float hmin = h.min();
    float hmax = h.max();
    h.remap(0.f, 1.f, hmin, hmax);

    hmap::transform(
        {&h},
        [post_gamma](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa = p_arrays[0];
          hmap::gamma_correction(*pa, post_gamma);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    h.remap(hmin, hmax, 0.f, 1.f);
  }

  // gain
  float post_gain = node.get_attr<FloatAttribute>("post_gain");

  if (post_gain != 1.f)
  {
    float hmin = h.min();
    float hmax = h.max();
    h.remap(0.f, 1.f, hmin, hmax);

    hmap::transform(
        {&h},
        [post_gain](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa = p_arrays[0];
          hmap::gain(*pa, post_gain);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    h.remap(hmin, hmax, 0.f, 1.f);
  }

  // smoothing
  const int ir = (int)(node.get_attr<FloatAttribute>("post_smoothing_radius") *
                       h.shape.x);

  if (ir)
  {
    hmap::transform(
        {&h},
        [&ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          return hmap::gpu::smooth_cpulse(*pa_out, ir);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    h.smooth_overlap_buffers();
  }

  // remap
  if (node.get_attr_ref<RangeAttribute>("post_remap")->get_is_active())
    h.remap(node.get_attr<RangeAttribute>("post_remap")[0],
            node.get_attr<RangeAttribute>("post_remap")[1]);

  // saturate
  if (node.get_attr_ref<RangeAttribute>("post_saturate")->get_is_active())
  {
    float hmin = h.min();
    float hmax = h.max();

    hmap::transform(
        {&h},
        [&node, &hmin, &hmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];

          float k = 0.1f; // TODO hardcoded?

          hmap::saturate(*pa_out,
                         node.get_attr<RangeAttribute>("post_saturate")[0],
                         node.get_attr<RangeAttribute>("post_saturate")[1],
                         hmin,
                         hmax,
                         k);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }
}

void setup_post_process_heightmap_attributes(BaseNode &node,
                                             bool      add_mix,
                                             bool      remap_active_state)
{
  Logger::log()->trace("setup_post_process_heightmap_attributes: [{}]/[{}]",
                       node.get_node_type(),
                       node.get_id());

  if (add_mix)
  {
    node.add_attr<EnumAttribute>("post_mix_method",
                                 "Mix Method",
                                 enum_mappings.blending_method_map,
                                 "replace");
    node.add_attr<FloatAttribute>("post_mix", "Mix Factor", 1.f, 0.f, 1.f);
  }

  node.add_attr<BoolAttribute>("post_inverse", "Invert Output", false);
  node.add_attr<FloatAttribute>("post_gamma", "Gamma", 1.f, 0.01f, 10.f);
  node.add_attr<FloatAttribute>("post_gain", "Gain", 1.f, 0.01f, 10.f);
  node.add_attr<FloatAttribute>("post_smoothing_radius",
                                "Smoothing Radius",
                                0.f,
                                0.f,
                                0.05f);
  node.add_attr<RangeAttribute>("post_remap", "Remap Range", remap_active_state);
  node.add_attr<RangeAttribute>("post_saturate", "Saturation Range", false);

  std::vector<std::string> *p_keys = node.get_attr_ordered_key_ref();

  p_keys->push_back("_GROUPBOX_BEGIN_Post-Processing");

  if (add_mix)
  {
    p_keys->push_back("post_mix_method");
    p_keys->push_back("post_mix");
  }

  p_keys->push_back("post_inverse");
  p_keys->push_back("post_gamma");
  p_keys->push_back("post_gain");
  p_keys->push_back("post_smoothing_radius");
  p_keys->push_back("post_remap");
  p_keys->push_back("post_saturate");
  p_keys->push_back("_GROUPBOX_END_");
}

} // namespace hesiod
