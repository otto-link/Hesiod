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

void add_export_button(BaseNode &node)
{
  Logger::log()->trace("add_export_button: node {}", node.get_label());

  // auto lambda = [](BaseNode &node)
  // {
  //   QPushButton *button = new QPushButton("Export!", &node);

  //   node.connect(
  //       button,
  //       &QPushButton::pressed,
  //       [&node]()
  //       {
  //         // bypass 'auto_export' attribute
  //         bool auto_export = node.get_attr<BoolAttribute>("auto_export");

  //         if (!auto_export)
  //           node.get_attr_ref<BoolAttribute>("auto_export")->set_value(true);

  //         node.compute();

  //         node.get_attr_ref<BoolAttribute>("auto_export")->set_value(auto_export);
  //       });

  //   return (QWidget *)button;
  // };

  // node.set_qwidget_fct(lambda);
}

} // namespace hesiod
