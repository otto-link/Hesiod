/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_sdf_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "path");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dx");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dy");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "sdf", CONFIG(node));

  // attribute(s)

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = false, .remap_active_state = true});
}

void compute_path_sdf_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_path = node.get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::VirtualArray *p_dx = node.get_value_ref<hmap::VirtualArray>("dx");
    hmap::VirtualArray *p_dy = node.get_value_ref<hmap::VirtualArray>("dy");
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("sdf");

    if (p_path->get_npoints() > 1)
    {
      hmap::for_each_tile(
          {p_out, p_dx, p_dy},
          [&node, p_path](std::vector<hmap::Array *> p_arrays,
                          const hmap::TileRegion    &region)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_dx = p_arrays[1];
            hmap::Array *pa_dy = p_arrays[2];

            glm::vec4 bbox_full = glm::vec4(0.f, 1.f, 0.f, 1.f);

            *pa_out = p_path->to_array_sdf(region.shape,
                                           bbox_full,
                                           pa_dx,
                                           pa_dy,
                                           region.bbox);
          },
          node.cfg().cm_cpu);

      // post-process
      post_process_heightmap(node, *p_out);
    }
    else
    {
      // fill with zeros
      hmap::for_each_tile(
          {p_out},
          [](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
          {
            hmap::Array *pa_out = p_arrays[0];
            *pa_out = 0.f;
          },
          node.cfg().cm_cpu);
    }
  }
}

} // namespace hesiod
