/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_kernel_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "heightmap");
  node.add_port<hmap::Array>(gnode::PortType::OUT,
                             "kernel",
                             node.get_config_ref()->shape);

  // attribute(s)
  node.add_attr<FloatAttribute>("radius", "radius", 0.1f, 0.001f, 0.2f);
  node.add_attr<BoolAttribute>("normalize", "normalize", false);
  node.add_attr<BoolAttribute>("envelope", "envelope", false);
  node.add_attr<EnumAttribute>("envelope_kernel",
                               "envelope_kernel",
                               enum_mappings.kernel_type_map,
                               "cubic_pulse");

  // attribute(s) order
  node.set_attr_ordered_key(
      {"radius", "normalize", "_SEPARATOR_", "envelope", "envelope_kernel"});
}

void compute_heightmap_to_kernel_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("heightmap");

  if (p_in)
  {
    hmap::Array *p_out = node.get_value_ref<hmap::Array>("kernel");

    int ir = std::max(
        1,
        (int)(node.get_attr<FloatAttribute>("radius") * node.get_config_ref()->shape.x));
    hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

    hmap::Array array = p_in->to_array();
    *p_out = array.resample_to_shape(kernel_shape);

    if (node.get_attr<BoolAttribute>("envelope"))
    {
      hmap::Array env = hmap::get_kernel(
          kernel_shape,
          (hmap::KernelType)node.get_attr<EnumAttribute>("envelope_kernel"));

      *p_out *= env;
    }

    if (node.get_attr<BoolAttribute>("normalize"))
      *p_out /= p_out->sum();
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
