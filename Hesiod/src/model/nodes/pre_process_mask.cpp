/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

std::shared_ptr<hmap::Heightmap> pre_process_mask(BaseNode         *p_node,
                                                  hmap::Heightmap *&p_mask,
                                                  hmap::Heightmap  &h)
{
  // do not modify any existing input mask and return a dummy shared
  // pointer that will not be used
  if (p_mask || !GET("mask_activate", BoolAttribute))
    return std::make_shared<hmap::Heightmap>();

  // create mask storage and assign to current mask pointer
  std::shared_ptr<hmap::Heightmap> sp_mask = std::make_shared<hmap::Heightmap>(CONFIG);
  p_mask = sp_mask.get();

  // --- mask definition

  const std::string mask_type = GET("mask_type", ChoiceAttribute);
  const int         ir = (int)(GET("mask_radius", FloatAttribute) * p_mask->shape.x);

  if (mask_type == "Elevation")
  {
    *p_mask = h;
    p_mask->remap();
  }
  else if (mask_type == "Elevation mid-range")
  {
    *p_mask = h;
    p_mask->remap();

    // mask <- h * (1 - h)
    hmap::transform(
        {p_mask},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          *pa_out *= (1.f - *pa_out);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }
  else if (mask_type == "Gradient norm")
  {
    hmap::transform(
        {p_mask, &h},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::gradient_norm(*pa_in);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_mask->remap();
  }
  else
  {
    LOG->error("pre_process_mask: unknown mask type: {}", mask_type);
  }

  // filter
  if (ir > 0)
    hmap::transform(
        {p_mask},
        [&ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::gpu::smooth_cpulse(*pa_out, ir);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

  p_mask->smooth_overlap_buffers();

  // --- apply gain to the mask

  float mask_gain = GET("mask_gain", FloatAttribute);

  if (mask_gain != 1.f)
    hmap::transform(
        {p_mask},
        [mask_gain](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa = p_arrays[0];
          hmap::gain(*pa, mask_gain);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

  if (GET("mask_inverse", BoolAttribute))
    p_mask->inverse();

  return sp_mask;
}

void setup_mask_attributes(BaseNode *p_node)
{
  ADD_ATTR(BoolAttribute, "mask_activate", false);

  std::vector<std::string> choices = {"Elevation",
                                      "Elevation mid-range",
                                      "Gradient norm"};
  ADD_ATTR(ChoiceAttribute, "mask_type", choices);

  ADD_ATTR(BoolAttribute, "mask_inverse", false);
  ADD_ATTR(FloatAttribute, "mask_radius", 0.01f, 0.f, 0.05f);
  ADD_ATTR(FloatAttribute, "mask_gain", 1.f, 0.f, 10.f);

  std::vector<std::string> *p_keys = p_node->get_attr_ordered_key_ref();

  p_keys->push_back("_SEPARATOR_");
  p_keys->push_back("mask_activate");
  p_keys->push_back("mask_type");
  p_keys->push_back("mask_radius");
  p_keys->push_back("mask_gain");
  p_keys->push_back("mask_inverse");
}

} // namespace hesiod
