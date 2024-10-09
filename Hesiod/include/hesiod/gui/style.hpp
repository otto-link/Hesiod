/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file base_node.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <string>

#include <QColor>

#include "highmap/array.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

namespace hesiod
{

static std::map<std::string, QColor> data_color_map = {
    {typeid(hmap::Array).name(), QColor(255, 121, 198, 255)},
    {typeid(hmap::Cloud).name(), QColor(139, 233, 253, 255)},
    {typeid(hmap::HeightMap).name(), QColor(248, 248, 242, 255)},
    {typeid(hmap::HeightMapRGBA).name(), QColor(255, 184, 108, 255)},
    {typeid(hmap::Path).name(), QColor(255, 184, 108, 255)},
};

static std::map<std::string, QColor> category_color_map = {
    {"Converter", QColor(188, 182, 163, 255)},
    {"Comment", QColor(170, 170, 170, 255)},
    {"Debug", QColor(200, 0, 0, 255)},
    {"Math", QColor(0, 43, 54, 255)},
    {"Geometry", QColor(101, 123, 131, 255)},
    {"Roads", QColor(147, 161, 161, 255)},
    {"Routing", QColor(188, 182, 163, 255)},
    {"IO", QColor(203, 196, 177, 255)},
    {"Features", QColor(181, 137, 0, 255)},
    {"Erosion", QColor(203, 75, 22, 255)},
    {"Mask", QColor(211, 54, 130, 255)},
    {"Filter", QColor(108, 113, 196, 255)},
    {"Operator", QColor(108, 113, 196, 255)},
    {"Hydrology", QColor(38, 139, 210, 255)},
    {"Primitive", QColor(42, 161, 152, 255)},
    {"Biomes", QColor(133, 153, 0, 255)},
    {"Texture", QColor(0, 0, 0, 255)},
};

} // namespace hesiod