/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QPushButton>

#include "hesiod/gui/widgets/data_preview.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/node_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

NodeWidget::NodeWidget(std::weak_ptr<BaseNode>   model,
                       QPointer<GraphNodeWidget> p_gnw,
                       QWidget                  *parent)
    : QWidget(parent), model(model), p_gnw(p_gnw)
{
  Logger::log()->trace("NodeWidget::NodeWidget");

  this->data_preview = new DataPreview(model, this);

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
                [this](const std::string &, const std::string &id)
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
                [this](const std::string &, const std::string &id)
                {
                  if (auto m = this->model.lock())
                  {
                    if (id == m->get_id())
                      this->on_compute_finished();
                  }
                });
}

QSize NodeWidget::sizeHint() const
{
  // TODO dynamic size
  return QSize(x, 128);
}

} // namespace hesiod
