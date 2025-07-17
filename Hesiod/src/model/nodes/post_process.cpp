/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void post_apply_enveloppe(BaseNode *p_node, hmap::Heightmap &h, hmap::Heightmap *p_env)
{
  LOG->trace("post_apply_enveloppe: [{}]/[{}]",
             p_node->get_node_type(),
             p_node->get_id());

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
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }
}

void post_process_heightmap(BaseNode         *p_node,
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
        p_node->get_config_ref()->hmap_transform_mode_gpu);

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

void post_process_heightmap(BaseNode *p_node, hmap::Heightmap &h, hmap::Heightmap *p_in)
{
  LOG->trace("post_process_heightmap: [{}]/[{}]",
             p_node->get_node_type(),
             p_node->get_id());

  // mix
  if (p_in)
  {
    // mix
    float k = 0.1f; // TODO hardcoded?
    int   ir = 0;
    int   method = GET("post_mix_method", EnumAttribute);
    blend_heightmaps(h, *p_in, h, static_cast<BlendingMethod>(method), k, ir);

    // lerp between input and output
    float t = GET("post_mix", FloatAttribute);

    hmap::transform(
        {&h, p_in},
        [t](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::lerp(*pa_in, *pa_out, t);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }

  // inverse
  if (GET("post_inverse", BoolAttribute))
    h.inverse();

  // saturate
  float post_gain = GET("post_gain", FloatAttribute);

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
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    h.remap(hmin, hmax, 0.f, 1.f);
  }

  // smoothing
  const int ir = (int)(GET("post_smoothing_radius", FloatAttribute) * h.shape.x);

  if (ir)
  {
    hmap::transform(
        {&h},
        [&ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          return hmap::gpu::smooth_cpulse(*pa_out, ir);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

    h.smooth_overlap_buffers();
  }

  // remap
  if (GET_MEMBER("post_remap", RangeAttribute, is_active))
    h.remap(GET("post_remap", RangeAttribute)[0], GET("post_remap", RangeAttribute)[1]);
}

void setup_post_process_heightmap_attributes(BaseNode *p_node, bool add_mix)
{
  LOG->trace("setup_post_process_heightmap_attributes: [{}]/[{}]",
             p_node->get_node_type(),
             p_node->get_id());

  if (add_mix)
  {
    ADD_ATTR(EnumAttribute, "post_mix_method", blending_method_map, "replace");
    ADD_ATTR(FloatAttribute, "post_mix", 1.f, 0.f, 1.f);
  }

  ADD_ATTR(BoolAttribute, "post_inverse", false);
  ADD_ATTR(FloatAttribute, "post_gain", 1.f, 0.01f, 10.f);
  ADD_ATTR(FloatAttribute, "post_smoothing_radius", 0.f, 0.f, 0.05f);
  ADD_ATTR(RangeAttribute, "post_remap");

  std::vector<std::string> *p_keys = p_node->get_attr_ordered_key_ref();

  p_keys->push_back("_SEPARATOR_");
  p_keys->push_back("_SEPARATOR_TEXT_Post-processing");

  if (add_mix)
  {
    p_keys->push_back("post_mix_method");
    p_keys->push_back("post_mix");
  }

  p_keys->push_back("post_inverse");
  p_keys->push_back("post_gain");
  p_keys->push_back("post_smoothing_radius");
  p_keys->push_back("post_remap");
}

} // namespace hesiod
