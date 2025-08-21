/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include <QLabel>
#include <QPushButton>

#include "attributes/widgets/abstract_widget.hpp"
#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/widgets/node_settings_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

NodeSettingsWidget::NodeSettingsWidget(GraphNodeWidget *p_graph_node_widget,
                                       QWidget         *parent)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget)
{
  LOG->trace("NodeSettingsWidget::NodeSettingsWidget");

  if (!p_graph_node_widget)
  {
    const std::string
        msg = "NodeSettingsWidget::NodeSettingsWidget: p_graph_node_widget is nullptr";
    LOG->critical(msg);
    throw std::invalid_argument(msg);
  }

  this->initialize_layout();
  this->setup_connections();
  this->update_content();
}

void NodeSettingsWidget::initialize_layout()
{
  LOG->trace("NodeSettingsWidget::initialize_layout");

  this->layout = new QVBoxLayout(this);

  QScrollArea *scroll_area = new QScrollArea(this);
  QWidget     *scroll_widget = new QWidget(scroll_area);

  this->scroll_layout = new QGridLayout(scroll_widget);
  this->scroll_layout->setAlignment(Qt::AlignTop);
  this->scroll_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

  scroll_widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
  scroll_widget->setLayout(this->scroll_layout);

  scroll_area->setWidgetResizable(true);
  scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scroll_area->setWidget(scroll_widget);

  this->layout->addWidget(scroll_area);
  this->setLayout(this->layout);
}

void NodeSettingsWidget::setup_connections()
{
  LOG->trace("NodeSettingsWidget::setup_connections");

  this->connect(p_graph_node_widget,
                &gngui::GraphViewer::selection_has_changed,
                this,
                &NodeSettingsWidget::update_content);

  // this covers also the case where the node settings has been
  // changed elsewhere (in the context-menu settings for instance)
  this->connect(p_graph_node_widget->get_p_graph_node(),
                &GraphNode::update_finished,
                [this]() { this->update_content(); });
}

void NodeSettingsWidget::update_content()
{
  LOG->trace("NodeSettingsWidget::update_content");

  if (!this->p_graph_node_widget)
  {
    LOG->error("NodeSettingsWidget::update_content: p_graph_node_widget "
               "is nullptr");
    return;
  }

  // empty and delete items of current layout
  clear_layout(this->scroll_layout);
  int row = 0;

  std::vector<std::string> selected_ids = this->p_graph_node_widget
                                              ->get_selected_node_ids();
  std::vector<std::string> all_ids = merge_unique(this->pinned_node_ids, selected_ids);

  // label
  QLabel *label = new QLabel("Node settings", this);
  this->scroll_layout->addWidget(label, row++, 0);

  for (auto &node_id : all_ids)
  {
    BaseNode *p_node = this->p_graph_node_widget->get_p_graph_node()
                           ->get_node_ref_by_id<BaseNode>(node_id);

    if (!p_node)
    {
      LOG->trace("NodeSettingsWidget::update_content: reference to node "
                 "{} is nullptr",
                 node_id);
      continue;
    }

    {
      const std::string txt = p_node->get_caption() + " (" + node_id + ")";

      // TODO move to gui_utils.hpp
      QWidget     *separator_widget = new QWidget;
      QHBoxLayout *sep_layout = new QHBoxLayout(separator_widget);
      sep_layout->setSpacing(0);
      sep_layout->setContentsMargins(0, 0, 0, 0);

      QFrame *leftLine = new QFrame;
      leftLine->setFrameShape(QFrame::HLine);
      leftLine->setFrameShadow(QFrame::Sunken);

      QLabel *label = new QLabel(txt.c_str());

      QFrame *rightLine = new QFrame;
      rightLine->setFrameShape(QFrame::HLine);
      rightLine->setFrameShadow(QFrame::Sunken);

      sep_layout->addWidget(leftLine);
      sep_layout->addWidget(label);
      sep_layout->addWidget(rightLine);

      sep_layout->setStretch(0, 1);
      sep_layout->setStretch(2, 6);

      this->scroll_layout->addWidget(separator_widget, row++, 0);
    }

    // preview
    DataPreview *p_data_preview = p_node->get_data_preview_ref();

    if (p_data_preview)
    {
      QPixmap preview_pixmap = p_data_preview->get_preview_pixmap();

      if (!preview_pixmap.isNull())
      {
        const int width = 48;
        QLabel   *label_preview = new QLabel(this);
        QPixmap   scaled_pixmap = preview_pixmap.scaled(width,
                                                      width,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);
        label_preview->setPixmap(scaled_pixmap);
        label_preview->setScaledContents(true);
        label_preview->setFixedSize(width, width);
        this->scroll_layout->addWidget(label_preview, row++, 0, Qt::AlignLeft);
      }
    }

    // pinned checkbox
    QPushButton *button_pin = new QPushButton("Pin this node");
    button_pin->setCheckable(true);
    button_pin->setChecked(contains(this->pinned_node_ids, node_id));
    this->scroll_layout->addWidget(button_pin, row++, 0);

    this->connect(button_pin,
                  &QPushButton::toggled,
                  [this, button_pin, node_id]()
                  {
                    if (button_pin->isChecked())
                      this->pinned_node_ids.push_back(node_id);
                    else
                      remove_all_occurrences(this->pinned_node_ids, node_id);
                  });

    // settings
    bool        add_save_reset_state_buttons = false;
    std::string window_title = "";

    attr::AttributesWidget *attributes_widget = new attr::AttributesWidget(
        p_node->get_attr_ref(),
        p_node->get_attr_ordered_key_ref(),
        window_title,
        add_save_reset_state_buttons);

    // attributes_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    attributes_widget->setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding);

    // change the attribute widget layout spacing a posteriori

    // TODO factorize
    QVBoxLayout *retrieved_layout = qobject_cast<QVBoxLayout *>(
        attributes_widget->layout());
    if (retrieved_layout)
    {
      retrieved_layout->setSpacing(0);
      retrieved_layout->setContentsMargins(0, 0, 0, 0);

      for (int i = 0; i < retrieved_layout->count(); ++i)
      {
        QWidget *child = retrieved_layout->itemAt(i)->widget();
        if (!child)
          continue;

        if (auto *inner_layout = child->layout())
        {
          inner_layout->setContentsMargins(32, 2, 64, 2);
          inner_layout->setSpacing(4);
        }
      }
    }

    this->scroll_layout->addWidget(attributes_widget, row++, 0);
  }
}

} // namespace hesiod
