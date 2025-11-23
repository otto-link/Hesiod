/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>

#include "hesiod/gui/widgets/data_preview.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/node_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

NodeWidget::NodeWidget(std::weak_ptr<BaseNode>   model,
                       QPointer<GraphNodeWidget> p_gnw,
                       const std::string        &msg,
                       QWidget                  *parent)
    : QWidget(parent), model(model), p_gnw(p_gnw), msg(msg)
{
  Logger::log()->trace("NodeWidget::NodeWidget");

  this->setup_layout();
  this->setup_connections();
}

void NodeWidget::on_compute_finished() { this->data_preview->update_preview(); }

void NodeWidget::on_compute_started() { this->data_preview->clear_preview(); }

void NodeWidget::setup_connections()
{
  if (!p_gnw)
    return;

  this->connect(p_gnw,
                &GraphNodeWidget::compute_started,
                this,
                [this](const std::string &id)
                {
                  if (auto m = this->model.lock())
                  {
                    if (id == m->get_id())
                      this->on_compute_started();
                  }
                });

  this->connect(p_gnw,
                &GraphNodeWidget::compute_finished,
                this,
                [this](const std::string &id)
                {
                  if (auto m = this->model.lock())
                  {
                    if (id == m->get_id())
                      this->on_compute_finished();
                  }
                });
}

void NodeWidget::setup_layout()
{
  this->layout = new QVBoxLayout(this);
  this->setLayout(this->layout);
  this->layout->setSpacing(0);
  this->layout->setContentsMargins(0, 0, 0, 0);

  this->data_preview = new DataPreview(model, this);
  this->layout->addWidget(this->data_preview);

  if (!this->msg.empty())
  {
    auto *label = new QLabel(msg.c_str());
    resize_font(label, -2);
    this->layout->addWidget(label);
  }
}

QSize NodeWidget::sizeHint() const { return QSize(128, 128); }

} // namespace hesiod
