/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>
#include <QVBoxLayout>

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void add_deprecated_warning_label(BaseNode *p_node, const std::string &msg = "")
{
  Logger::log()->trace("add_deprecated_warning_label: node {}", p_node->get_label());

  auto lambda = [msg](BaseNode *p_node)
  {
    if (!p_node)
      return (QWidget *)nullptr;

    std::string str = "!!! Deprecated !!!\n" + msg;
    QLabel     *label = new QLabel(str.c_str(), p_node);
    label->setStyleSheet("color: #ff5555;");
    resize_font(label, -2);
    return (QWidget *)label;
  };

  p_node->set_qwidget_fct(lambda);
}

} // namespace hesiod
