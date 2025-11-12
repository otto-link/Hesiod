/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_to_vectors_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<std::vector<float>>(gnode::PortType::OUT, "x");
  p_node->add_port<std::vector<float>>(gnode::PortType::OUT, "y");
  p_node->add_port<std::vector<float>>(gnode::PortType::OUT, "v");

  // attribute(s)

  // attribute(s) order
  p_node->set_attr_ordered_key({});
}

void compute_cloud_to_vectors_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_in = p_node->get_value_ref<hmap::Cloud>("cloud");

  if (p_in)
  {
    auto p_x = p_node->get_value_ref<std::vector<float>>("x");
    auto p_y = p_node->get_value_ref<std::vector<float>>("y");
    auto p_v = p_node->get_value_ref<std::vector<float>>("v");

    *p_x = p_in->get_x();
    *p_y = p_in->get_y();
    *p_v = p_in->get_values();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
