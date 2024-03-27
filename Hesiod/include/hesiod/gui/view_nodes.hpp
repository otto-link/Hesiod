/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSpinBox>
#include <QWidgetAction>

#include <QtNodes/NodeData>

#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/noise.hpp"

namespace hesiod
{

/**
 * @brief ViewNoise class, noise primitive generation, with GUI features.
 */
class ViewNoise : public Noise
{
public:
  ViewNoise(const ModelConfig &config) : Noise(config, false)
  {
    this->image_preview = ImagePreview(config);
    this->compute();
  }

  void context_menu(const QPointF /* pos */) override
  {
    LOG_DEBUG("context menu in ViewNoise");
    QMenu *menu = new QMenu();

    QLineEdit lineEdit;
    lineEdit.setText("Sample Text");
    QWidgetAction *widgetAction = new QWidgetAction(menu);
    widgetAction->setDefaultWidget(&lineEdit);
    menu->addAction(widgetAction);

    QSpinBox      *spinbox = new QSpinBox();
    QWidgetAction *layout_action = new QWidgetAction(menu);
    layout_action->setDefaultWidget((QWidget *)spinbox);
    menu->addAction(layout_action);

    menu->exec(QCursor::pos());
  }

  QWidget *embeddedWidget() override { return (QWidget *)this->image_preview.label; }

private:
  ImagePreview image_preview;

  void compute() override
  {
    Noise::compute();
    this->image_preview.update(this->out.get());
  }
};

} // namespace hesiod