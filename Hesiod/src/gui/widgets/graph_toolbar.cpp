/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/graph_toolbar.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

GraphToolbar::GraphToolbar(QPointer<GraphNodeWidget> p_graph_node_widget, QWidget *parent)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget)
{
  Logger::log()->trace("GraphToolbar::GraphToolbar");

  if (!this->p_graph_node_widget)
    return;

  this->setAttribute(Qt::WA_StyledBackground, true);

  this->setup_layout();

  // listen to external config changes
  this->connect(this->p_graph_node_widget,
                &GraphNodeWidget::config_changed,
                this,
                &GraphToolbar::sync_resolution_from_config);
}

void GraphToolbar::on_resolution_button_clicked(QAbstractButton *button)
{
  if (!this->p_graph_node_widget)
    return;

  int res = button->property("resolution").toInt();
  this->p_graph_node_widget->apply_new_config(res);
}

void GraphToolbar::setup_layout()
{
  Logger::log()->trace("GraphToolbar::setup_layout");

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(2, 0, 2, 2);
  layout->setSpacing(4);

  layout->addStretch();

  this->resolution_group = new QButtonGroup(this);
  this->resolution_group->setExclusive(true);

  AppContext &ctx = HSD_CTX;
  std::string style =
      "QPushButton { background: COLOR_BG_DEEP; color: COLOR_TEXT_PRIMARY; border: 0px; "
      "border-radius: 6px; } QPushButton:checked { color: COLOR_ACCENT; "
      "font-weight: bold; } QPushButton:hover { color: COLOR_BW_ACCENT; }";

  replace_all(style,
              "COLOR_BG_DEEP",
              ctx.app_settings.colors.bg_deep.name().toStdString());
  replace_all(style,
              "COLOR_TEXT_PRIMARY",
              ctx.app_settings.colors.text_primary.name().toStdString());
  replace_all(style, "COLOR_ACCENT", ctx.app_settings.colors.accent.name().toStdString());
  replace_all(style,
              "COLOR_BW_ACCENT",
              ctx.app_settings.colors.accent_bw.name().toStdString());

  constexpr int resolutions[] = {512, 1024, 2048, 4096};

  for (int res : resolutions)
  {
    auto *b = new QPushButton(res == 1024   ? "1k"
                              : res == 2048 ? "2k"
                              : res == 4096 ? "4k"
                                            : "512");

    b->setCheckable(true);
    b->setFixedWidth(32);
    b->setToolTip("Change resolution.");
    b->setProperty("resolution", res);
    b->setStyleSheet(style.c_str());
    resize_font(b, -2);

    this->resolution_group->addButton(b);
    layout->addWidget(b);
  }

  this->connect(resolution_group,
                &QButtonGroup::buttonClicked,
                this,
                &GraphToolbar::on_resolution_button_clicked);

  // initialize UI from config
  this->sync_resolution_from_config();
}

void GraphToolbar::sync_resolution_from_config()
{
  if (!this->p_graph_node_widget)
    return;

  auto *gno = p_graph_node_widget->get_p_graph_node();
  if (!gno)
    return;

  auto *p_config = gno->get_config_ref();
  if (!p_config)
    return;

  int current_res = p_config->shape.x;

  for (auto *button : this->resolution_group->buttons())
  {
    int res = button->property("resolution").toInt();
    button->setChecked(res == current_res);
  }
}

} // namespace hesiod
