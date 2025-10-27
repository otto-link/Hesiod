/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>
#include <QVBoxLayout>

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void add_wip_warning_label(BaseNode *p_node)
{
  Logger::log()->trace("add_wip_warning_label: node {}", p_node->get_label());

  auto lambda = [](BaseNode *p_node)
  {
    QLabel *label = new QLabel("Work in progress\nNode subject to change", p_node);
    label->setStyleSheet("color: #ffb86c;");
    resize_font(label, -2);
    return (QWidget *)label;
  };

  p_node->set_qwidget_fct(lambda);
}

} // namespace hesiod
