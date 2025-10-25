/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/convolve.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_convolve_svd_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Array>(gnode::PortType::IN, "kernel");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "rank", 4, 1, 8);

  // attribute(s) order
  p_node->set_attr_ordered_key({"rank"});

  setup_post_process_heightmap_attributes(p_node, true);
}

void compute_convolve_svd_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");
  hmap::Array     *p_kernel = p_node->get_value_ref<hmap::Array>("kernel");

  if (p_in && p_kernel)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        *p_out,
        *p_in,
        [p_node, p_kernel](hmap::Array &out, hmap::Array &in)
        { out = hmap::convolve2d_svd(in, *p_kernel, GET("rank", IntAttribute)); });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
