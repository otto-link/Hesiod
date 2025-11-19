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

void setup_path_dig_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "path");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "width", 0.001f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "decay", 0.001f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "flattening_radius", 0.001f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "depth", 0.f, -0.2f, 0.2f);
  ADD_ATTR(node, BoolAttribute, "force_downhill", false);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"width", "decay", "flattening_radius", "depth", "force_downhill"});
}

void compute_path_dig_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path      *p_path = node.get_value_ref<hmap::Path>("path");
  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_path && p_in)
    if (p_path->get_npoints() > 1)
    {
      hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

      *p_out = *p_in;

      int ir_width = (int)(GET(node, "width", FloatAttribute) * p_out->shape.x);
      int ir_decay = (int)(GET(node, "decay", FloatAttribute) * p_out->shape.x);
      int ir_flattening_radius = (int)(GET(node, "flattening_radius", FloatAttribute) *
                                       p_out->shape.x);

      ir_width = std::max(1, ir_width);
      ir_decay = std::max(1, ir_decay);
      ir_flattening_radius = std::max(1, ir_flattening_radius);

      if (!GET(node, "force_downhill", BoolAttribute))
      {
        hmap::transform(*p_out,
                        [&node, p_path, ir_width, ir_decay, ir_flattening_radius](
                            hmap::Array      &z,
                            hmap::Vec4<float> bbox)
                        {
                          hmap::dig_path(z,
                                         *p_path,
                                         ir_width,
                                         ir_decay,
                                         ir_flattening_radius,
                                         GET(node, "force_downhill", BoolAttribute),
                                         bbox,
                                         GET(node, "depth", FloatAttribute));
                        });
      }
      else
      {
        // TODO if downhill is activated, so far not distributed
        hmap::Array z_array = p_out->to_array();

        hmap::dig_path(z_array,
                       *p_path,
                       ir_width,
                       ir_decay,
                       ir_flattening_radius,
                       GET(node, "force_downhill", BoolAttribute),
                       hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f), // bbox
                       GET(node, "depth", FloatAttribute));

        p_out->from_array_interp(z_array);
      }

      p_out->smooth_overlap_buffers();
    }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
