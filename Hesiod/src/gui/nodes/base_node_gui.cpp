/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/base_node.hpp"

namespace hesiod
{

void BaseNode::context_menu(const QPointF /* pos */)
{
  if (!this->qmenu)
  {
    // menu populated here to ensure this is done by the derived class, not by the
    // current base class BaseNode (or the attribute mapping will be empty)
    AttributesWidget *attributes_widget = new AttributesWidget(&this->attr,
                                                               &this->attr_ordered_key);
    this->qmenu = new QMenu();
    // this->qmenu.setStyleSheet("QMenu::item { padding: 5px; }");

    {
      QLabel *label = new QLabel(this->caption());
      QFont   f = label->font();
      f.setBold(true);
      label->setFont(f);
      QWidgetAction *widget_action = new QWidgetAction(qmenu);
      widget_action->setDefaultWidget(label);
      this->qmenu->addAction(widget_action);
    }

    this->qmenu->addSeparator();

    {
      QWidgetAction *widget_action = new QWidgetAction(qmenu);
      widget_action->setDefaultWidget(attributes_widget);
      this->qmenu->addAction(widget_action);
    }

    connect(attributes_widget,
            &AttributesWidget::value_changed,
            [this]() { this->compute(); });
  }

  this->qmenu->popup(QCursor::pos());
}

QWidget *BaseNode::embeddedWidget()
{
  if (!this->preview)
  {
    this->preview = new Preview(this);
    connect(this, &NodeDelegateModel::dataUpdated, this->preview, &Preview::update_image);
  }
  return (QWidget *)this->preview;
}

} // namespace hesiod
