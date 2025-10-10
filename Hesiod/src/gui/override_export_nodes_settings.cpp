/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>
#include <random>

#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void override_export_nodes_settings(const std::string           &fname,
                                    const std::filesystem::path &export_path,
                                    uint                         random_seeds_increment)
{
  Logger::log()->trace("override_export_nodes_settings: fname = {}, export_path = {}",
                       fname,
                       export_path.string());

  std::random_device rd;

  // load
  nlohmann::json json = json_from_file(fname);

  // modify
  for (auto &[key, value] : json["graph_manager"]["graph_nodes"].items())
    for (auto &j : value["nodes"])
    {
      std::string node_label = j["label"];

      if (node_label.find("Export") != std::string::npos)
      {
        // force node auto export
        if (j.contains("auto_export"))
          j["auto_export"]["value"] = true;

        // change export name
        if (j.contains("fname") && j["fname"].contains("value"))
        {
          std::filesystem::path
              basename = j["fname"]["value"].get<std::filesystem::path>().filename();

          std::string label = j.value("label", "");
          std::string id = j.value("id", "");

          std::string           new_name = label + "_" + id + "_" + basename.string();
          std::filesystem::path new_path = export_path / new_name;

          j["fname"]["value"] = new_path.string();
        }
      }

      if (random_seeds_increment > 0)
      {
        if (j.contains("seed") && j["seed"].contains("value"))
        {
          uint v = j["seed"]["value"];
          j["seed"]["value"] = v + random_seeds_increment;
        }
      }
    }

  // write back
  json_to_file(json, fname);
}

} // namespace hesiod
