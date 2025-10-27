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

void setup_thru_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(BoolAttribute, "block_update", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"block_update"});

  // specialized GUI
  auto lambda = [](BaseNode *p_node)
  {
    bool state = GET("block_update", BoolAttribute);

    QPushButton *button = new QPushButton("BLOCK UPDATE", p_node);
    button->setCheckable(true);
    button->setChecked(state);
    button->setStyleSheet("QPushButton:checked { background-color: red; }");

    p_node->connect(button,
                    &QPushButton::pressed,
                    [p_node, button]()
                    {
                      bool new_state = !GET("block_update", BoolAttribute);
                      GET_REF("block_update", BoolAttribute)->set_value(new_state);
                      button->setChecked(new_state);
                      p_node->get_p_graph()->update(p_node->get_id());
                    });

    return (QWidget *)button;
  };

  p_node->set_qwidget_fct(lambda);
}

void compute_thru_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in && !GET("block_update", BoolAttribute))
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
