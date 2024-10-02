/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/authoring.hpp"
#include "hesiod/model/nodes/export.hpp"
#include "hesiod/model/nodes/filters.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/primitives.hpp"
#include "hesiod/model/nodes/range.hpp"

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
      {"Noise", "Primitive/Coherent"},
      {"NoiseFbm", "Primitive/Coherent"},
      {"ExportHeightmap", "IO/Files"},
      {"GammaCorrection", "Filter/Recurve"},
      {"Remap", "Filter/Range"}};

  return node_inventory;
}

std::shared_ptr<gnode::Node> node_factory(const std::string           &node_type,
                                          std::shared_ptr<ModelConfig> config)
{
  // clang-format off
  switch(str2int(node_type.c_str()))
  {
    HSD_NODE_CASE(Brush);
    HSD_NODE_CASE(Cloud);
    HSD_NODE_CASE(GammaCorrection);
    HSD_NODE_CASE(ExportHeightmap);
    HSD_NODE_CASE(Noise);
    HSD_NODE_CASE(NoiseFbm);
    HSD_NODE_CASE(Remap);
  default:
    // clang-format on
    throw std::invalid_argument("Unknown node type in node_factory: " + node_type);
  }
}

} // namespace hesiod
