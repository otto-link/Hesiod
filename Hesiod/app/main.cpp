/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <memory>

#include <QObject>

#include "hesiod/logger.hpp"
#include "hesiod/model/model_config.hpp"

#include "hesiod/model/graph_node.hpp"

#include "hesiod/model/nodes/primitives.hpp"
#include "hesiod/model/nodes/range.hpp"

int main() // (int argc, char *argv[])
{
  HLOG->info("Welcome to Hesiod v{}.{}.{}!",
             HESIOD_VERSION_MAJOR,
             HESIOD_VERSION_MINOR,
             HESIOD_VERSION_PATCH);

  auto config = std::make_shared<hesiod::ModelConfig>();
  config->log_debug();

  hesiod::GraphNode graph = hesiod::GraphNode("graph1", config);

  auto id1 = graph.add_node<hesiod::Noise>(config);
  auto id2 = graph.add_node<hesiod::Remap>(config);
  auto id3 = graph.add_node<hesiod::Remap>(config);

  graph.new_link(id1, "out", id2, "input");
  graph.new_link(id1, "out", id3, "input");

  graph.update(id1);

  graph.get_node_ref_by_id<hesiod::Noise>(id1)->get_proxy_ref()->log_debug();

  // nlohmann::json json = graph.json_to();
  // graph.clear();
  // graph.print();
  // graph.json_from(json);
  // graph.print();
  // graph.update(id1);

  return 0;
}
