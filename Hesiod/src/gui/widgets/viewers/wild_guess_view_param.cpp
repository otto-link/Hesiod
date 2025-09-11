/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <typeinfo>

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/gui/widgets/viewers/viewer.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

std::string helper_get_preferred_port_inout(const BaseNode       &node,
                                            const std::type_info &type)
{
  std::string value;
  int         in_candidate = -1;

  for (int k = 0; k < node.get_nports(); ++k)
  {
    if (node.get_data_type(k) == type.name())
    {
      if (node.get_port_type(k) == gngui::PortType::OUT)
      {
        value = node.get_port_label(k);
        break; // OUT has priority
      }
      else if (in_candidate == -1 && node.get_port_type(k) == gngui::PortType::IN)
      {
        in_candidate = k;
      }
    }
  }

  if (value.empty() && in_candidate != -1)
    value = node.get_port_label(in_candidate);

  return value;
}

std::string helper_get_preferred_port_label(const BaseNode       &node,
                                            const std::type_info &type,
                                            const std::string    &label)
{
  std::string value;

  for (int k = 0; k < node.get_nports(); ++k)
    if (node.get_port_label(k) == label && node.get_data_type(k) == type.name())
    {
      value = node.get_port_label(k);
      break;
    }

  return value;
}

void wild_guess_view_param(ViewerNodeParam &view_param,
                           const ViewerType & /* viewer_type */,
                           const BaseNode &node)
{
  for (auto &[key, value] : view_param.port_ids)
  {
    if (key == "elevation")
    {
      value = helper_get_preferred_port_inout(node, typeid(hmap::Heightmap));
    }

    if (key == "color")
    {
      value = helper_get_preferred_port_label(node,
                                              typeid(hmap::HeightmapRGBA),
                                              "texture");
    }

    if (key == "normal_map")
    {
      value = helper_get_preferred_port_label(node,
                                              typeid(hmap::HeightmapRGBA),
                                              "normal map");
    }

    if (key == "points")
    {
      value = helper_get_preferred_port_inout(node, typeid(hmap::Cloud));
    }

    if (key == "path")
    {
      value = helper_get_preferred_port_inout(node, typeid(hmap::Path));
    }
  }
}

} // namespace hesiod
