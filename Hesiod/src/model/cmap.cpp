/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "hesiod/model/cmap.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace hesiod
{

static std::map<std::string, int> available_loaded_colormaps_mapping = {};
static std::map<int, std::vector<std::vector<float>>> available_loaded_colormaps = {};

bool load_colormap_data_from_file(std::filesystem::path cmap_path)
{
  if(std::filesystem::exists(cmap_path) == false) return false;

  std::ifstream cmapStream = std::ifstream(cmap_path);

  if(cmapStream.bad()) return false;

  nlohmann::json cmapParsedData = nlohmann::json();

  cmapStream >> cmapParsedData;

  int currentIndex = 0;

  for(auto [key, data] : cmapParsedData.items())
  {
    available_loaded_colormaps_mapping.emplace(key, currentIndex);
    available_loaded_colormaps.emplace(currentIndex, data.get<std::vector<std::vector<float>>>());

    currentIndex++;
  }

  return true;
}

std::vector<std::vector<float>> get_colormap_data(int cmap)
{
  return available_loaded_colormaps.contains(cmap) ? available_loaded_colormaps.at(cmap) : std::vector<std::vector<float>>();
}

std::map<std::string, int> get_colormap_mapping()
{
  return available_loaded_colormaps_mapping;
}

} // namespace hesiod
