/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QPushButton>
#include <QVBoxLayout>

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void add_export_button(BaseNode *p_node)
{
  LOG->trace("add_export_button: node {}", p_node->get_label());

  auto lambda = [](BaseNode *p_node)
  {
    QWidget *widget = new QWidget(p_node);

    QVBoxLayout *layout = new QVBoxLayout(p_node);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    widget->setLayout(layout);

    layout->addWidget(p_node->get_data_preview_ref());

    QPushButton *button = new QPushButton("Export!", p_node);
    layout->addWidget(button);

    p_node->connect(button,
                    &QPushButton::pressed,
                    [p_node]()
                    {
                      // bypass 'auto_export' attribute
                      bool auto_export = GET("auto_export", BoolAttribute);

                      if (!auto_export)
                        GET_REF("auto_export", BoolAttribute)->set_value(true);

                      p_node->compute();

                      GET_REF("auto_export", BoolAttribute)->set_value(auto_export);
                    });

    return widget;
  };

  p_node->set_qwidget_fct(lambda);
}

} // namespace hesiod
