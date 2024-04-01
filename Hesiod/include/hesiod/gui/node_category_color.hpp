/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <map>

#include <QColor>

namespace hesiod
{

static std::map<std::string, QColor> node_category_color = {
    {"Converter", QColor(136, 192, 208, 255)},
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
    {"Texture", QColor(0, 0, 0, 255)}};

}