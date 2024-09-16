/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "gnodegui/style.hpp"

#include "hesiod/graph_editor.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

GraphEditor::GraphEditor(const std::string           &id,
                         std::shared_ptr<ModelConfig> config,
                         bool                         headless)
    : GraphNode(id, config)
{
  HLOG->trace("GraphEditor::GraphEditor, graph id {}", this->get_id());

  if (headless)
  {
    HLOG->trace(
        "GraphEditor::GraphEditor, running in headless mode for the graph editor");
  }
  else
  {
    HLOG->trace("GraphEditor::GraphEditor, initializing graph GUI");

    // --- instantiate

    this->viewer = std::make_unique<gngui::GraphViewer>(this->get_id());

    std::map<std::string, std::string> ni = {{"Noise", "Primitive/Coherent"},
                                             {"Remap", "Filter/Range"}};
    this->viewer->set_node_inventory(ni);

    // --- connect with core node graph (GraphNode)

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::connection_deleted,
                  this,
                  &GraphEditor::on_connection_deleted);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::connection_finished,
                  this,
                  &GraphEditor::on_connection_finished);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_clear_request,
                  this,
                  &GraphEditor::on_graph_clear_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_reload_request,
                  this,
                  &GraphEditor::on_graph_reload_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::new_node_request,
                  this,
                  &GraphEditor::on_new_node_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::node_deleted,
                  this,
                  &GraphEditor::on_node_deleted_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::node_reload_request,
                  this,
                  &GraphEditor::on_node_reload_request);
  }

  // --- syles

  // TODO move somewhere else
  // GN_STYLE->node.color_port_data[] = QColor(189, 147, 249, 255);

  GN_STYLE->node.color_category = {{"Converter", QColor(188, 182, 163, 255)},
                                   {"Comment", QColor(170, 170, 170, 255)},
                                   {"Debug", QColor(200, 0, 0, 255)},
                                   {"Math", QColor(0, 43, 54, 255)},
                                   {"Geometry", QColor(101, 123, 131, 255)},
                                   {"Roads", QColor(147, 161, 161, 255)},
                                   {"Routing", QColor(188, 182, 163, 255)},
                                   {"IO", QColor(203, 196, 177, 255)},
                                   {"Features", QColor(181, 137, 0, 255)},
                                   {"Erosion", QColor(203, 75, 22, 255)},
                                   {"Mask", QColor(211, 54, 130, 255)},
                                   {"Filter", QColor(108, 113, 196, 255)},
                                   {"Operator", QColor(108, 113, 196, 255)},
                                   {"Hydrology", QColor(38, 139, 210, 255)},
                                   {"Primitive", QColor(42, 161, 152, 255)},
                                   {"Biomes", QColor(133, 153, 0, 255)}};
}

void GraphEditor::on_connection_deleted(const std::string &id_out,
                                        const std::string &port_id_out,
                                        const std::string &id_in,
                                        const std::string &port_id_in)
{
  HLOG->trace("GraphEditor::on_connection_deleted, {}:{} -> {}:{}",
              id_out,
              port_id_out,
              id_in,
              port_id_in);

  this->remove_link(id_out, port_id_out, id_in, port_id_in);
  this->update(id_in);
}

void GraphEditor::on_connection_finished(const std::string &id_out,
                                         const std::string &port_id_out,
                                         const std::string &id_in,
                                         const std::string &port_id_in)
{
  HLOG->trace("GraphEditor::on_connection_finished, {}:{} -> {}:{}",
              id_out,
              port_id_out,
              id_in,
              port_id_in);

  this->new_link(id_out, port_id_out, id_in, port_id_in);
  this->update(id_out);
}

void GraphEditor::on_graph_clear_request()
{
  HLOG->trace("GraphEditor::on_graph_clear_request");
  this->clear();
  if (this->viewer)
    this->viewer->clear();
}

void GraphEditor::on_graph_reload_request()
{
  HLOG->trace("GraphEditor::on_graph_reload_request");
  this->update();
  // TODO signals back to GUI before / after
}

void GraphEditor::on_new_node_request(const std::string &node_type, QPointF scene_pos)
{
  if (node_type == "")
    return;

  std::string node_id = this->new_node(node_type);

  if (this->viewer)
  {
    BaseNode *p_node = this->get_node_ref_by_id<BaseNode>(node_id);
    this->viewer->add_node(p_node->get_proxy_ref(), scene_pos);
  }
}

void GraphEditor::on_node_deleted_request(const std::string &node_id)
{
  HLOG->trace("GraphNode::on_node_deleted_request, node {}", node_id);

  this->remove_node(node_id);
  if (this->viewer)
    this->viewer->remove_node(node_id);
}

void GraphEditor::on_node_reload_request(const std::string &node_id)
{
  HLOG->trace("GraphNode::on_node_reload_request, node {}", node_id);
  this->update(node_id);
  // TODO signals back to GUI before / after
}

} // namespace hesiod
