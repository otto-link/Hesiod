/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

// -----------------------------------------------------------------------------
// Ports & Attributes
// -----------------------------------------------------------------------------

constexpr const char *P_BACKGROUND = "background";
constexpr const char *P_OUT = "cloud";

constexpr const char *A_CLOUD = "cloud";

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup_cloud_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // --- Ports

  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_BACKGROUND);
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, P_OUT);

  // --- Attributes
  node.add_attr<CloudAttribute>(A_CLOUD, "Cloud");

  // --- Attribute(s) order

  node.set_attr_ordered_key(
      {"_GROUPBOX_BEGIN_Main Parameters", A_CLOUD, "_GROUPBOX_END_"});

  setup_background_image_for_cloud_attribute(node, A_CLOUD, P_BACKGROUND);
}

// -----------------------------------------------------------------------------
// Compute
// -----------------------------------------------------------------------------

void compute_cloud_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // --- Inputs / Outputs

  auto *p_out = node.get_value_ref<hmap::Cloud>(P_OUT);

  // --- Params

  const auto cloud_attr = node.get_attr<CloudAttribute>(A_CLOUD);

  // --- Compute

  *p_out = hmap::Cloud(cloud_attr);
}

} // namespace hesiod
