/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_kernel_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "heightmap");
  p_node->add_port<hmap::Array>(gnode::PortType::OUT,
                                "kernel",
                                p_node->get_config_ref()->shape);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.1f, 0.001f, 0.2f, "radius");
  p_node->add_attr<BoolAttribute>("normalize", false, "normalize");
  p_node->add_attr<BoolAttribute>("envelope", false, "envelope");
  p_node->add_attr<MapEnumAttribute>("envelope_kernel",
                                     "cubic_pulse",
                                     kernel_type_map,
                                     "envelope_kernel");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"radius", "normalize", "_SEPARATOR_", "envelope", "envelope_kernel"});
}

void compute_heightmap_to_kernel_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("heightmap");

  if (p_in)
  {
    hmap::Array *p_out = p_node->get_value_ref<hmap::Array>("kernel");

    int ir = std::max(
        1,
        (int)(GET("radius", FloatAttribute) * p_node->get_config_ref()->shape.x));
    hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

    hmap::Array array = p_in->to_array();
    *p_out = array.resample_to_shape(kernel_shape);

    if (GET("envelope", BoolAttribute))
    {
      hmap::Array env = hmap::get_kernel(
          kernel_shape,
          (hmap::KernelType)GET("envelope_kernel", MapEnumAttribute));

      *p_out *= env;
    }

    if (GET("normalize", BoolAttribute))
      *p_out /= p_out->sum();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
