/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

void post_apply_enveloppe(BaseNode *p_node, hmap::Heightmap &h, hmap::Heightmap *p_env)
{
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

std::vector<std::string> split_string(const std::string &string, char delimiter)
{
  std::vector<std::string> result;
  std::stringstream        ss(string);
  std::string              word;

  while (std::getline(ss, word, delimiter))
    result.push_back(word);

  return result;
}

} // namespace hesiod
