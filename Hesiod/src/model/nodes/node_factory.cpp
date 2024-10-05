/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

namespace hesiod
{

// HELPER
constexpr unsigned int str2int(const char *str, int h = 0)
{
  // https://stackoverflow.com/questions/16388510
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

std::map<std::string, std::string> get_node_inventory()
{
  std::map<std::string, std::string> node_inventory = {
      {"Brush", "Primitive/Authoring"},
      {"Cloud", "Geometry/Cloud"},
      {"ExportHeightmap", "IO/Files"},
      {"GammaCorrection", "Filter/Recurve"},
      {"Gradient", "Math/Gradient"},
      {"Noise", "Primitive/Coherent"},
      {"NoiseFbm", "Primitive/Coherent"},
      {"Path", "Geometry/Path"},
      {"Remap", "Filter/Range"}};

  return node_inventory;
}

std::shared_ptr<gnode::Node> node_factory(const std::string           &node_type,
                                          std::shared_ptr<ModelConfig> config)
{
  auto sptr = std::make_shared<hesiod::BaseNode>(node_type, config);

  switch (str2int(node_type.c_str()))
  {
    SETUP_NODE(Brush, brush);
    SETUP_NODE(Cloud, cloud);
    SETUP_NODE(ExportHeightmap, export_heightmap);
    SETUP_NODE(GammaCorrection, gamma_correction);
    SETUP_NODE(Gradient, gradient);
    SETUP_NODE(Noise, noise);
    SETUP_NODE(NoiseFbm, noise_fbm);
    SETUP_NODE(Path, path);
    SETUP_NODE(Remap, remap);
  default:
    throw std::invalid_argument("Unknown node type in node_factory: " + node_type);
  }

  return sptr;
}

} // namespace hesiod
