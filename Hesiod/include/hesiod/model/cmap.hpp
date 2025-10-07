/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <map>
#include <string>
#include <vector>

#define HSD_COLORMAP_FILE "data/cmap.json"

namespace hesiod
{

struct CmapData
{
  std::string                     id = "";
  std::string                     label = "";
  int                             id_int = 0;
  std::vector<std::vector<float>> colors = {};
};

class CmapManager
{
public:
  // get the singleton instance
  static CmapManager &get_instance()
  {
    static CmapManager instance;
    return instance;
  }

  std::vector<std::vector<float>> get_colormap_data(int cmap_id);
  std::map<std::string, int>      get_colormap_name_mapping();
  void                            update_colormap_data(bool append = false);

private:
  // private constructor
  CmapManager();

  // delete copy constructor and assignment operator to enforce singleton
  CmapManager(const CmapManager &) = delete;
  CmapManager &operator=(const CmapManager &) = delete;

  // store colormap
  std::map<std::string, CmapData> cmaps;

  int count_id = 0;
};

} // namespace hesiod
