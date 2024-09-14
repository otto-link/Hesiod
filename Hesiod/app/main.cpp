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

  hesiod::Noise node = hesiod::Noise("n1", config);
  hesiod::Remap node2 = hesiod::Remap("n2", config);
  node.compute();
  node2.compute();

  node.json_to();
  node2.json_to();

  hesiod::GraphNode graph = hesiod::GraphNode("graph1", config);

  return 0;
}
