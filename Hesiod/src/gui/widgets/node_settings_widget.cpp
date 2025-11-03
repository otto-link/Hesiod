/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QScrollBar>

#include "attributes/widgets/abstract_widget.hpp"
#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/node_settings_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

NodeSettingsWidget::NodeSettingsWidget(GraphNodeWidget *p_graph_node_widget,
                                       QWidget         *parent)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget)
{
  Logger::log()->trace("NodeSettingsWidget::NodeSettingsWidget");

  if (!p_graph_node_widget)
  {
    const std::string
        msg = "NodeSettingsWidget::NodeSettingsWidget: p_graph_node_widget is nullptr";
    Logger::log()->critical(msg);
    throw std::invalid_argument(msg);
  }
}

void NodeSettingsWidget::initialize_layout()
{
  Logger::log()->trace("NodeSettingsWidget::initialize_layout");

  this->layout = new QVBoxLayout(this);

  this->scroll_area = new QScrollArea(this);
  QWidget *scroll_widget = new QWidget(this->scroll_area);

  // central layout that hosts all settings rows
  this->scroll_layout = new QGridLayout(scroll_widget);
  this->scroll_layout->setAlignment(Qt::AlignTop);

  // Allow default constraint so Qt can compute sensible min/max sizes
  this->scroll_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
  this->scroll_layout->setContentsMargins(8, 0, 8, 0);
  this->scroll_layout->setSpacing(0);

  // Make the scroll widget expand vertically so the scroll area will show scrollbars
  scroll_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  scroll_widget->setLayout(this->scroll_layout);

  this->scroll_area->setWidgetResizable(true);
  this->scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->scroll_area->setWidget(scroll_widget);

  // make sure the single column stretches to occupy the width
  this->scroll_layout->setColumnStretch(0, 1);

  this->layout->addWidget(this->scroll_area);
  this->setLayout(this->layout);
}

void NodeSettingsWidget::setup_connections()
{
  Logger::log()->trace("NodeSettingsWidget::setup_connections");

  // Defensive local QPointer wrappers so we don't call through dangling raw pointers
  QPointer<GraphNodeWidget> graph_widget_ptr(this->p_graph_node_widget);

  if (!graph_widget_ptr)
  {
    Logger::log()->error(
        "NodeSettingsWidget::setup_connections: p_graph_node_widget is nullptr");
    return;
  }

  // connect selection changes -> update UI
  this->connect(graph_widget_ptr,
                &gngui::GraphViewer::selection_has_changed,
                this,
                &NodeSettingsWidget::update_content);

  // connect to the underlying graph node update signal, if available
  if (auto p_graph = graph_widget_ptr->get_p_graph_node())
  {
    // use QPointer to ensure safety if the graph node is deleted elsewhere
    QPointer<GraphNode> graph_ptr(p_graph);
    this->connect(
        graph_ptr,
        &GraphNode::update_finished,
        this,
        [this, graph_ptr]()
        {
          if (!graph_ptr)
          {
            Logger::log()->warn(
                "NodeSettingsWidget: graph node destroyed before update callback");
            return;
          }
          this->update_content();
        });
  }
  else
  {
    Logger::log()->warn(
        "NodeSettingsWidget::setup_connections: underlying GraphNode is nullptr");
  }
}

void NodeSettingsWidget::update_content()
{
  Logger::log()->trace("NodeSettingsWidget::update_content");

  if (this->prevent_content_update)
  {
    Logger::log()->trace("NodeSettingsWidget::update_content: prevented");
    return;
  }

  // create widget content here (not in the constructor) to ensure the
  // the parent widget is setup
  if (this->first_pass)
  {
    this->initialize_layout();
    this->setup_connections();
    this->first_pass = false;
  }

  // some guard access
  if (!this->p_graph_node_widget)
  {
    Logger::log()->error(
        "NodeSettingsWidget::update_content: p_graph_node_widget is nullptr");
    return;
  }

  if (!this->scroll_layout)
  {
    Logger::log()->error("NodeSettingsWidget::update_content: scroll_layout is nullptr");
    return;
  }

  auto p_graph = this->p_graph_node_widget->get_p_graph_node();
  if (!p_graph)
  {
    Logger::log()->warn(
        "NodeSettingsWidget::update_content: get_p_graph_node returned nullptr");
    return;
  }

  // for scroll_area size
  QWidget *parent = this->parentWidget();
  if (parent)
    this->scroll_area->setMinimumHeight((int)(0.9f * parent->size().height()));
  else
    this->scroll_area->setMinimumHeight(512);

  // empty and delete items of current layout
  clear_layout(this->scroll_layout);
  int row = 0;

  std::vector<std::string> selected_ids = this->p_graph_node_widget
                                              ->get_selected_node_ids();
  std::vector<std::string> all_ids = merge_unique(this->pinned_node_ids, selected_ids);

  // header label
  QLabel *title_label = new QLabel("Node settings", /*parent=*/this->parentWidget());
  title_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  this->scroll_layout->addWidget(title_label, row++, 0);

  for (auto &node_id : all_ids)
  {
    // Defensive retrieval of node reference
    BaseNode *p_node = p_graph->get_node_ref_by_id<BaseNode>(node_id);

    if (!p_node)
    {
      Logger::log()->trace(
          "NodeSettingsWidget::update_content: reference to node {} is nullptr",
          node_id);
      continue;
    }

    // separator (with parent so Qt will manage its lifetime)
    const QString txt = QString::fromStdString(p_node->get_caption() + " (" + node_id +
                                               ")");
    QWidget      *separator_widget = new QWidget(/*parent=*/this);
    QHBoxLayout  *sep_layout = new QHBoxLayout(separator_widget);
    sep_layout->setSpacing(0);
    sep_layout->setContentsMargins(0, 0, 0, 0);

    QFrame *left_line = new QFrame(separator_widget);
    left_line->setFrameShape(QFrame::HLine);
    left_line->setFrameShadow(QFrame::Sunken);

    QLabel *sep_label = new QLabel(txt, separator_widget);
    sep_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QFrame *right_line = new QFrame(separator_widget);
    right_line->setFrameShape(QFrame::HLine);
    right_line->setFrameShadow(QFrame::Sunken);

    sep_layout->addWidget(left_line);
    sep_layout->addWidget(sep_label);
    sep_layout->addWidget(right_line);

    sep_layout->setStretch(0, 1);
    sep_layout->setStretch(2, 6);

    this->scroll_layout->addWidget(separator_widget, row++, 0);

    // preview (use scroll_widget as parent so lifetime is tied to the container)
    DataPreview *p_data_preview = p_node->get_data_preview_ref();

    if (p_data_preview)
    {
      QPixmap preview_pixmap = p_data_preview->get_preview_pixmap();

      if (!preview_pixmap.isNull())
      {
        const int width = 48;
        QLabel   *label_preview = new QLabel(/*parent=*/this);
        QPixmap   scaled_pixmap = preview_pixmap.scaled(width,
                                                      width,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);
        label_preview->setPixmap(scaled_pixmap);
        label_preview->setScaledContents(true);
        label_preview->setFixedSize(width, width);
        label_preview->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->scroll_layout->addWidget(label_preview, row++, 0, Qt::AlignLeft);
      }
    }

    // pinned checkbox button
    QPushButton *button_pin = new QPushButton("Pin this node", this);
    button_pin->setCheckable(true);
    button_pin->setChecked(contains(this->pinned_node_ids, node_id));
    this->scroll_layout->addWidget(button_pin, row++, 0);

    // Use QPointer in the lambda to defend against deletion
    QPointer<QPushButton> bp(button_pin);
    this->connect(
        button_pin,
        &QPushButton::toggled,
        this,
        [this, bp, node_id]()
        {
          if (!bp)
          {
            Logger::log()->warn(
                "NodeSettingsWidget: pin button destroyed before toggle handler");
            return;
          }

          if (bp->isChecked())
          {
            if (!contains(this->pinned_node_ids, node_id))
              this->pinned_node_ids.push_back(node_id);
          }
          else
          {
            remove_all_occurrences(this->pinned_node_ids, node_id);
          }

          Logger::log()->trace("NodeSettingsWidget: pinned nodes now count={}",
                               this->pinned_node_ids.size());
        });

    // attr widget
    attr::AttributesWidget *attributes_widget = this->p_graph_node_widget
                                                    ->get_node_attributes_widget(node_id);

    if (!attributes_widget)
      continue;

    this->scroll_layout->addWidget(attributes_widget, row++, 0);
  }
}

} // namespace hesiod
