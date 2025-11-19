/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QPushButton>

#include "highmap/filters.hpp"

#include "gnode/graph.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_thru_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<BoolAttribute>("block_update", "block_update", false);

  // attribute(s) order
  node.set_attr_ordered_key({"block_update"});

  // specialized GUI
  auto lambda = [](BaseNode &node)
  {
    bool state = node.get_attr<BoolAttribute>("block_update");

    QPushButton *button = new QPushButton("BLOCK UPDATE", &node);
    button->setCheckable(true);
    button->setChecked(state);
    button->setStyleSheet("QPushButton:checked { background-color: red; }");

    node.connect(button,
                 &QPushButton::pressed,
                 [&node, button]()
                 {
                   bool new_state = !node.get_attr<BoolAttribute>("block_update");
                   node.get_attr_ref<BoolAttribute>("block_update")->set_value(new_state);
                   button->setChecked(new_state);
                   node.get_p_graph()->update(node.get_id());
                 });

    return (QWidget *)button;
  };

  node.set_qwidget_fct(lambda);
}

void compute_thru_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in && !node.get_attr<BoolAttribute>("block_update"))
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
