/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/gui/widgets/node_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

QWidget *node_widget_factory(const std::string        &node_type,
                             std::weak_ptr<BaseNode>   model,
                             QPointer<GraphNodeWidget> p_gnw,
                             QWidget                  *parent)
{
  std::string msg = "";

  // adjust msg
  std::vector<std::string> wip_nodes = {"CoastalErosionDiffusion",
                                        "MountainCone",
                                        "MountainInselberg",
                                        "MountainStump",
                                        "MountainTibesti",
                                        "NormalMapToHeightmap",
                                        "Strata",
                                        "Recurve",
                                        "Rifts",
                                        "ShatteredPeak",
                                        "WaterDepthFromMask"};
  std::vector<std::string> deprecated_nodes = {"ColorizeCmap",
                                               "Stratify",
                                               "StratifyMultiscale",
                                               "StratifyOblique",
                                               "TextureAdvectionWarp"};

  if (contains<std::string>(wip_nodes, node_type))
    msg = "Work in progress\nNode subject to change";

  if (contains<std::string>(deprecated_nodes, node_type))
    msg = "!!! Deprecated !!!";

  // create node
  if (node_type.starts_with("Export"))
  {
    return new ExportNodeWidget(model, p_gnw, msg, parent);
  }
  else if (node_type == "Debug")
  {
    return new DebugNodeWidget(model, p_gnw, msg, parent);
  }
  else if (node_type == "Toggle")
  {
    return new ToggleNodeWidget(model, p_gnw, msg, parent);
  }
  else if (node_type == "Thru")
  {
    return new ThruNodeWidget(model, p_gnw, msg, parent);
  }

  // default
  return new NodeWidget(model, p_gnw, msg, parent);
}

} // namespace hesiod
