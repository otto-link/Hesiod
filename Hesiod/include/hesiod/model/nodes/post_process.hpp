/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file post_process.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "highmap/heightmap.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

void post_apply_enveloppe(BaseNode *p_node, hmap::Heightmap &h, hmap::Heightmap *p_env);

void post_process_heightmap(BaseNode         *p_node,
                            hmap::Heightmap  &h,
                            bool              inverse,
                            bool              smoothing,
                            float             smoothing_radius,
                            bool              saturate,
                            hmap::Vec2<float> saturate_range,
                            float             saturate_k,
                            bool              remap,
                            hmap::Vec2<float> remap_range);

void post_process_heightmap(BaseNode *p_node, hmap::Heightmap &h);

void setup_post_process_heightmap_attributes(BaseNode *p_node);

// --- mask preprocessing

std::shared_ptr<hmap::Heightmap> pre_process_mask(BaseNode         *p_node,
                                                  hmap::Heightmap *&p_mask,
                                                  hmap::Heightmap  &h);

void setup_pre_process_mask_attributes(BaseNode *p_node);

} // namespace hesiod
