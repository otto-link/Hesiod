/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QStyle>
#include <QVBoxLayout>

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/node_info_dialog.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"

namespace hesiod
{

NodeInfoDialog::NodeInfoDialog(GraphNodeWidget   *p_graph_node_widget,
                               const std::string &node_id,
                               QWidget           *parent)
    : QDialog(parent), p_graph_node_widget(p_graph_node_widget), node_id(node_id)
{
  Logger::log()->trace("NodeInfoDialog::NodeInfoDialog");

  if (!this->p_graph_node_widget)
  {
    Logger::log()->error(
        "NodeInfoDialog::NodeInfoDialog: p_graph_node_widget is nullptr");
    return;
  }

  this->setWindowTitle("Hesiod - Node information");
  this->setMinimumWidth(256);
  this->setWindowModality(Qt::NonModal);
  this->setWindowFlags(Qt::Window); // ensures full top-level behavior
  this->setAttribute(Qt::WA_DeleteOnClose);

  this->setup_layout();
  this->setup_connections();
  this->update_content();
}

NodePointers NodeInfoDialog::get_node_pointers() const
{
  if (!this->p_graph_node_widget)
    return NodePointers();

  return {this->p_graph_node_widget->get_p_graph_node()->get_node_ref_by_id<BaseNode>(
              this->node_id),
          this->p_graph_node_widget->get_graphics_node_by_id(this->node_id)};
}

void NodeInfoDialog::on_comment_text_changed()
{
  auto ptrs = this->get_node_pointers();

  if (!ptrs.node || !ptrs.gfx || !this->editor)
    return;

  ptrs.node->set_comment(this->editor->toPlainText().toStdString());
  ptrs.gfx->update_proxy_widget();
}

void NodeInfoDialog::setup_connections()
{
  Logger::log()->trace("NodeInfoDialog::setup_connections");

  if (!this->p_graph_node_widget)
    return;

  // GraphNodeWidget -> this (make sure the dialog is closed when
  // the graph is destroyed or if the node is deleted)
  this->connect(this->p_graph_node_widget,
                &QObject::destroyed,
                this,
                [this]()
                {
                  this->p_graph_node_widget = nullptr;
                  this->deleteLater();
                });

  this->connect(this->p_graph_node_widget,
                &gngui::GraphViewer::node_deleted,
                this,
                [this](const std::string &deleted_node_id)
                {
                  if (deleted_node_id == this->node_id)
                    this->deleteLater();
                });

  // update when the graph node is modified (the node itself of the
  // links)
  this->connect(
      this->p_graph_node_widget->get_p_graph_node(),
      &GraphNode::compute_finished,
      this,
      [this](const std::string & /* graph_id */, const std::string &updated_node_id)
      {
        if (updated_node_id == this->node_id)
          this->update_content();
      });

  this->connect(this->p_graph_node_widget,
                &gngui::GraphViewer::connection_deleted,
                this,
                [this](const std::string &id_out,
                       const std::string &,
                       const std::string &,
                       const std::string &,
                       bool)
                {
                  if (id_out == this->node_id)
                    this->update_content();
                });

  this->connect(this->p_graph_node_widget,
                &gngui::GraphViewer::connection_finished,
                this,
                [this](const std::string &id_out,
                       const std::string &,
                       const std::string &,
                       const std::string &)
                {
                  if (id_out == this->node_id)
                    this->update_content();
                });
}

void NodeInfoDialog::setup_layout()
{
  Logger::log()->trace("NodeInfoDialog::setup_layout");

  // --- safeguards

  if (!this->p_graph_node_widget)
    return;

  auto ptrs = this->get_node_pointers();
  if (!ptrs.node || !ptrs.gfx)
    return;

  // --- layout

  this->layout = new QVBoxLayout(this);

  // --- main label

  {
    std::string str = ptrs.node->get_caption() + "/" + ptrs.node->get_id();
    QLabel     *label = new QLabel(str.c_str());
    this->layout->addWidget(label);
  }

  // --- ports

  {
    QLabel *label = new QLabel("Ports");
    layout->addWidget(label);

    QWidget    *container = new QWidget(this);
    std::string style = std::format(
        "background-color: {};",
        HSD_CTX.app_settings.colors.bg_deep.name().toStdString());
    container->setStyleSheet(style.c_str());
    this->grid_ports = new QGridLayout(container);

    layout->addWidget(container);
  }

  // --- comment

  {
    QLabel *label = new QLabel("Comment");
    layout->addWidget(label);

    this->editor = new QPlainTextEdit();
    this->editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    this->editor->setPlainText(ptrs.node->get_comment().c_str());
    this->editor->setMinimumHeight(64);
    layout->addWidget(this->editor);

    this->connect(this->editor,
                  &QPlainTextEdit::textChanged,
                  this,
                  &NodeInfoDialog::on_comment_text_changed);
  }

  // --- exit

  {
    QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
    this->connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    this->layout->addWidget(button_box);
  }
}

void NodeInfoDialog::update_content()
{
  Logger::log()->trace("NodeInfoDialog::update_content");
  this->update_ports_content();
}

void NodeInfoDialog::update_ports_content()
{
  Logger::log()->trace("NodeInfoDialog::update_ports_content");

  auto ptrs = this->get_node_pointers();
  if (!ptrs.node || !ptrs.gfx || !this->editor)
    return;

  // empty and delete items of current layout
  clear_layout(this->grid_ports);

  // fill-in data to display
  struct Row
  {
    std::string caption;
    bool        is_connected;
    std::string type;
    std::string data_type;
    std::string data_info;
  };

  std::vector<Row> rows;

  for (int k = 0; k < ptrs.node->get_nports(); k++)
  {
    Row new_row;
    new_row.caption = ptrs.node->get_port_caption(k);
    new_row.is_connected = ptrs.node->is_port_connected(k);

    std::string str_ct = new_row.is_connected ? "✓" : " ";
    std::string str_in = std::format("→[{}] ", str_ct);
    std::string str_out = std::format(" [{}]→", str_ct);

    new_row.type = (ptrs.node->get_port_type(k) == gngui::PortType::IN) ? str_in
                                                                        : str_out;
    new_row.data_type = map_type_name(ptrs.node->get_data_type(k));

    // data info
    {
      // TODO make a dedicated function
      new_row.data_info = "-";

      if (ptrs.node->get_data_type(k) == typeid(hmap::Heightmap).name())
      {
        hmap::Heightmap *p_h = ptrs.node->get_value_ref<hmap::Heightmap>(new_row.caption);
        if (p_h)
          new_row.data_info = std::format("[{:.3e}, {:.3e}]", p_h->min(), p_h->max());
      }
      else if (ptrs.node->get_data_type(k) == typeid(hmap::Cloud).name())
      {
        hmap::Cloud *p_c = ptrs.node->get_value_ref<hmap::Cloud>(new_row.caption);
        if (p_c)
          new_row.data_info = std::format("[{:.3e}, {:.3e}], count: {}",
                                          p_c->get_values_min(),
                                          p_c->get_values_max(),
                                          p_c->get_npoints());
      }
      else if (ptrs.node->get_data_type(k) == typeid(hmap::Path).name())
      {
        hmap::Path *p_c = ptrs.node->get_value_ref<hmap::Path>(new_row.caption);
        if (p_c)
          new_row.data_info = std::format("[{:.3e}, {:.3e}], count: {}",
                                          p_c->get_values_min(),
                                          p_c->get_values_max(),
                                          p_c->get_npoints());
      }
    }

    rows.push_back(new_row);
  }

  // build layout
  QFont f("DejaVu Sans Mono", 9);

  int row_idx = 0;
  for (const Row &r : rows)
  {
    int col_idx = 0;
    for (const auto &s : {r.type, r.caption, r.data_type, r.data_info})
    {
      QLabel *label = new QLabel(s.c_str());
      label->setFont(f);
      this->grid_ports->addWidget(label, row_idx, col_idx);
      col_idx++;
    }
    row_idx++;
  }
}

} // namespace hesiod
