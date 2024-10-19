/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_to_heightmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "path");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "heightmap", CONFIG);

  // attribute(s)
  p_node->add_attr<BoolAttribute>("filled", false, "filled");
  p_node->add_attr<BoolAttribute>("remap", true, "remap");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<BoolAttribute>("smoothing", false, "smoothing");
  p_node->add_attr<FloatAttribute>("smoothing_radius",
                                   0.05f,
                                   0.f,
                                   0.2f,
                                   "smoothing_radius");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"filled", "_SEPARATOR_", "remap", "inverse", "smoothing", "smoothing_radius"});
}

void compute_path_to_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path *p_path = p_node->get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("heightmap");

    if (p_path->get_npoints() > 1)
    {
      if (!GET("filled", BoolAttribute))
      {
        hmap::fill(*p_out,
                   [p_path](hmap::Vec2<int> shape, hmap::Vec4<float> bbox)
                   {
                     hmap::Array z = hmap::Array(shape);
                     p_path->to_array(z, bbox);
                     return z;
                   });
      }
      else
      {
        // work on a single array as a temporary solution
        hmap::Array       z_array = hmap::Array(p_out->shape);
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);

        p_path->to_array(z_array, bbox, true);
        p_out->from_array_interp_nearest(z_array);
      }

      // post-process
      post_process_heightmap(*p_out,
                             GET("inverse", BoolAttribute),
                             GET("smoothing", BoolAttribute),
                             GET("smoothing_radius", FloatAttribute),
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             GET("remap", BoolAttribute),
                             {0.f, 1.f});
    }
    else
      // fill with zeros
      hmap::transform(*p_out, [](hmap::Array &x) { x = 0.f; });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
