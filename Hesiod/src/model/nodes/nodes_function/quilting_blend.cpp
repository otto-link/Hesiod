/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/synthesis.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_quilting_blend_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 3");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input 4");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("patch_width", 0.3f, 0.1f, 1.f, "patch_width");
  p_node->add_attr<FloatAttribute>("overlap", 0.9f, 0.05f, 0.95f, "overlap");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<BoolAttribute>("patch_flip", true, "patch_flip");
  p_node->add_attr<BoolAttribute>("patch_rotate", true, "patch_rotate");
  p_node->add_attr<BoolAttribute>("patch_transpose", true, "patch_transpose");
  p_node->add_attr<FloatAttribute>("filter_width_ratio",
                                   0.5f,
                                   0.f,
                                   1.f,
                                   "filter_width_ratio");

  // attribute(s) order
  p_node->set_attr_ordered_key({"patch_width",
                                "overlap",
                                "seed",
                                "patch_flip",
                                "patch_rotate",
                                "patch_transpose",
                                "filter_width_ratio"});
}

void compute_quilting_blend_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("input 2");
  hmap::Heightmap *p_in3 = p_node->get_value_ref<hmap::Heightmap>("input 3");
  hmap::Heightmap *p_in4 = p_node->get_value_ref<hmap::Heightmap>("input 4");

  std::vector<hmap::Heightmap *> ptr_list = {};
  for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
    if (ptr)
      ptr_list.push_back(ptr);

  if ((int)ptr_list.size())
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int ir = std::max(1, (int)(GET("patch_width", FloatAttribute) * p_out->shape.x));
    hmap::Vec2<int> patch_base_shape = hmap::Vec2<int>(ir, ir);

    // --- work on a single array (i.e. not-tiled algo)

    std::vector<hmap::Array>   arrays = {};
    std::vector<hmap::Array *> p_arrays = {};

    for (auto &ptr : {p_in1, p_in2, p_in3, p_in4})
      if (ptr)
        arrays.push_back(std::move(ptr->to_array()));

    for (auto &a : arrays)
      p_arrays.push_back(&a);

    hmap::Array out_array = hmap::quilting_blend(
        p_arrays,
        patch_base_shape,
        GET("overlap", FloatAttribute),
        GET("seed", SeedAttribute),
        GET("patch_flip", BoolAttribute),
        GET("patch_rotate", BoolAttribute),
        GET("patch_transpose", BoolAttribute),
        GET("filter_width_ratio", FloatAttribute));

    p_out->from_array_interp_nearest(out_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
