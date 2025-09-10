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

void wild_guess_view_param(ViewerNodeParam &view_param,
                           const ViewerType & /* viewer_type */,
                           const BaseNode &node)
{
  for (auto &[key, value] : view_param.port_ids)
  {
    if (key == "elevation")
    {
      // take the first output
      for (int k = 0; k < node.get_nports(); ++k)
        if (node.get_port_type(k) == gngui::PortType::OUT &&
            node.get_data_type(k) == typeid(hmap::Heightmap).name())
        {
          value = node.get_port_label(k);
          break;
        }
    }

    if (key == "color")
    {
      // take the first output
      for (int k = 0; k < node.get_nports(); ++k)
        if (node.get_port_label(k) == "texture" &&
            node.get_data_type(k) == typeid(hmap::HeightmapRGBA).name())
        {
          value = node.get_port_label(k);
          break;
        }
    }

    if (key == "normal_map")
    {
      // take the first output
      for (int k = 0; k < node.get_nports(); ++k)
        if (node.get_port_label(k) == "normal map" &&
            node.get_data_type(k) == typeid(hmap::HeightmapRGBA).name())
        {
          value = node.get_port_label(k);
          break;
        }
    }

    if (key == "points")
    {
      // take the first output
      for (int k = 0; k < node.get_nports(); ++k)
        if (node.get_port_type(k) == gngui::PortType::OUT &&
            node.get_data_type(k) == typeid(hmap::Cloud).name())
        {
          value = node.get_port_label(k);
          break;
        }
    }

    if (key == "path")
    {
      // take the first output
      for (int k = 0; k < node.get_nports(); ++k)
        if (node.get_port_type(k) == gngui::PortType::OUT &&
            node.get_data_type(k) == typeid(hmap::Path).name())
        {
          value = node.get_port_label(k);
          break;
        }
    }
  }
}

} // namespace hesiod
