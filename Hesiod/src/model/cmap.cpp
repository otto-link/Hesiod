/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"

namespace hesiod
{

CmapManager::CmapManager()
{
  LOG->info("initializing colormaps manager");
  this->update_colormap_data();
}

void CmapManager::update_colormap_data(bool append)
{
  if (!append)
  {
    this->cmaps.clear();
    this->count_id = 0;
  }

  // load data from json file
  std::string fname = HSD_COLORMAP_FILE;

  nlohmann::json json;
  std::ifstream  file(fname);

  if (file.is_open())
  {
    file >> json;
    file.close();
    LOG->trace("JSON successfully loaded from {}", fname);

    for (auto &[key, val] : json.items())
    {
      // LOG->trace("loading colormap: {}", key);

      CmapData cdata;
      cdata.id = key;
      cdata.id_int = this->count_id;
      cdata.label = "UNDEFINED"; // val["label"];
      cdata.colors = val["colors"].get<std::vector<std::vector<float>>>();

      this->cmaps[key] = cdata;
      this->count_id++;
    }
  }
  else
    LOG->error("Could not open file {} to load JSON", fname);
}

std::vector<std::vector<float>> CmapManager::get_colormap_data(int cmap_id)
{
  for (auto &[id, cmap_data] : this->cmaps)
    if (cmap_data.id_int == cmap_id)
      return cmap_data.colors;

  return {{0.f, 0.f, 0.f}};
}

std::map<std::string, int> CmapManager::get_colormap_name_mapping()
{
  std::map<std::string, int> mapping = {};
  for (auto &[id, cmap_data] : this->cmaps)
    mapping[id] = cmap_data.id_int;
  return mapping;
}

} // namespace hesiod
