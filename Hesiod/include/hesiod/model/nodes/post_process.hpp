/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once

#include "highmap/heightmap.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

struct PostProcessHeightmapOptions
{
  bool add_mix = false;
  bool remap_active_state = true;
};

void blend_heightmaps(hmap::Heightmap &h_out,
                      hmap::Heightmap &h1,
                      hmap::Heightmap &h2,
                      BlendingMethod   method,
                      float            k = 0.f,
                      int              ir = 0,
                      float            w1 = 1.f,
                      float            w2 = 1.f);

void post_apply_enveloppe(BaseNode &node, hmap::Heightmap &h, hmap::Heightmap *p_env);

void post_process_heightmap(BaseNode         &node,
                            hmap::Heightmap  &h,
                            bool              inverse,
                            bool              smoothing,
                            float             smoothing_radius,
                            bool              saturate,
                            hmap::Vec2<float> saturate_range,
                            float             saturate_k,
                            bool              remap,
                            hmap::Vec2<float> remap_range);

void post_process_heightmap(BaseNode        &node,
                            hmap::Heightmap &h,
                            hmap::Heightmap *p_in = nullptr);

void setup_post_process_heightmap_attributes(BaseNode                   &node,
                                             PostProcessHeightmapOptions options);

// --- mask preprocessing

std::shared_ptr<hmap::Heightmap> pre_process_mask(BaseNode         &node,
                                                  hmap::Heightmap *&p_mask,
                                                  hmap::Heightmap  &h);

void setup_pre_process_mask_attributes(BaseNode &node);

} // namespace hesiod
