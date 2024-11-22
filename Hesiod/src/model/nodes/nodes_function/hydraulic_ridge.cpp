/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_ridge_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("talus_global", 16.f, 0.01f, 32.f, "talus_global");
  p_node->add_attr<FloatAttribute>("intensity", 0.5f, 0.f, 2.f, "intensity");
  p_node->add_attr<FloatAttribute>("erosion_factor", 1.5f, 0.f, 4.f, "erosion_factor");
  p_node->add_attr<FloatAttribute>("smoothing_factor",
                                   0.5f,
                                   0.01f,
                                   2.f,
                                   "smoothing_factor");
  p_node->add_attr<FloatAttribute>("noise_ratio", 0.1f, 0.f, 1.f, "noise_ratio");
  p_node->add_attr<FloatAttribute>("radius", 0.01f, 0.f, 0.1f, "radius");
  p_node->add_attr<SeedAttribute>("seed");

  // attribute(s) order
  p_node->set_attr_ordered_key({"talus_global",
                                "intensity",
                                "erosion_factor",
                                "smoothing_factor",
                                "noise_ratio",
                                "radius",
                                "seed"});
}

void compute_hydraulic_ridge_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // --- work on a single array (as a temporary solution?)
    hmap::Array z_array = p_in->to_array();

    // handle masking
    hmap::Array *p_mask_array = nullptr;
    hmap::Array  mask_array;

    if (p_mask)
    {
      mask_array = p_mask->to_array();
      p_mask_array = &mask_array;
    }

    // backup amplitude for post-process remapping
    float zmin = z_array.min();
    float zmax = z_array.max();

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;
    int   ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::hydraulic_ridge(z_array,
                          talus,
                          p_mask_array,
                          GET("intensity", FloatAttribute),
                          GET("erosion_factor", FloatAttribute),
                          GET("smoothing_factor", FloatAttribute),
                          GET("noise_ratio", FloatAttribute),
                          ir,
                          GET("seed", SeedAttribute));

    hmap::remap(z_array, zmin, zmax);

    p_out->from_array_interp_nearest(z_array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
