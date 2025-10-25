/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QPushButton>
#include <QVBoxLayout>

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void add_export_button(BaseNode *p_node)
{
  Logger::log()->trace("add_export_button: node {}", p_node->get_label());

  auto lambda = [](BaseNode *p_node)
  {
    QPushButton *button = new QPushButton("Export!", p_node);

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

    return (QWidget *)button;
  };

  p_node->set_qwidget_fct(lambda);
}

} // namespace hesiod
