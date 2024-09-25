/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <memory>

#include <QApplication>
#include <QFont>

#include "hesiod/graph_editor.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/model_config.hpp"

#include "hesiod/model/nodes/primitives.hpp"
#include "hesiod/model/nodes/range.hpp"

int main(int argc, char *argv[])
{
  LOG->info("Welcome to Hesiod v{}.{}.{}!",
            HESIOD_VERSION_MAJOR,
            HESIOD_VERSION_MINOR,
            HESIOD_VERSION_PATCH);

  QApplication app(argc, argv);

  QFont font("Roboto");
  font.setPointSize(10);
  app.setFont(font);

  auto config = std::make_shared<hesiod::ModelConfig>();

  hesiod::GraphEditor graph = hesiod::GraphEditor("graph1", config);

  graph.on_new_node_request("Noise", QPointF(200, 200));
  graph.on_new_node_request("Remap", QPointF(400, 200));
  graph.on_new_node_request("GammaCorrection", QPointF(600, 200));
  graph.get_p_viewer()->zoom_to_content();

  graph.get_p_viewer()->resize(1000, 800);
  graph.get_p_viewer()->show();

  return app.exec();

  // hesiod::GraphNode graph = hesiod::GraphNode("graph1", config);
  // graph.get_node_ref_by_id<hesiod::Noise>(id1)->get_proxy_ref()->log_debug();

  // // nlohmann::json json = graph.json_to();
  // // graph.clear();
  // // graph.print();
  // // graph.json_from(json);
  // // graph.print();
  // // graph.update(id1);

  return 0;
}
