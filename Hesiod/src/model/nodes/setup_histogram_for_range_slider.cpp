/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/operator.hpp"
#include "highmap/virtual_array/virtual_array.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_histogram_for_range_attribute(BaseNode          &node,
                                         const std::string &attribute_key,
                                         const std::string &port_id)
{
  Logger::log()->trace("setup_histogram_for_range_slider: node {}", node.get_label());

  // function to compute the histogram
  auto lambda = [&node, port_id]()
  {
    std::pair<std::vector<float>, std::vector<float>> hist;

    hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>(port_id);

    if (p_in)
    {
      float vmin = p_in->min(node.cfg().cm_cpu);
      float vmax = p_in->max(node.cfg().cm_cpu);

      if (vmin != vmax)
      {
        int   nbins = p_in->shape.x;
        float a = 1.f / (vmax - vmin) * (float)(nbins - 1);
        float b = -vmin / (vmax - vmin) * (float)(nbins - 1);

        hmap::Array array = p_in->to_array({256, 256}, node.cfg().cm_cpu);

        // values
        bool endpoint = false;
        hist.first = hmap::linspace(vmin, vmax, nbins, endpoint);

        // cumul
        hist.second.resize(nbins);

        for (int j = 0; j < array.shape.y; j++)
          for (int i = 0; i < array.shape.x; i++)
            hist.second[(int)(a * array(i, j) + b)] += 1;
      }
    }

    return hist;
  };

  // assign function to attr
  node.get_attr_ref<RangeAttribute>(attribute_key)->set_histogram_fct(lambda);
  node.get_attr_ref<RangeAttribute>(attribute_key)->set_autorange(true);
}

} // namespace hesiod
