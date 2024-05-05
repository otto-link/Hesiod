/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file colormaps.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <map>
#include <string>
#include <vector>
#include <filesystem>

namespace hesiod
{

bool load_colormap_data_from_file(std::filesystem::path cmap_path);
std::vector<std::vector<float>> get_colormap_data(int cmap);
std::map<std::string, int> get_colormap_mapping();

} // namespace hesiod
