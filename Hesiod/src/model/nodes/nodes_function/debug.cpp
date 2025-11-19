/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>

#include "highmap/heightmap.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void setup_debug_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // no attribute(s)

  // specialized GUI
  auto lambda = [](BaseNode &node)
  {
    QLabel *label = new QLabel("DEBUG", &node);
    label->setAlignment(Qt::AlignLeft);

    node.connect(&node,
                 &BaseNode::compute_finished,
                 [label, &node]()
                 {
                   hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

                   std::string msg = "DEBUG\n";

                   if (p_in)
                   {
                     float min = p_in->min();
                     float max = p_in->max();
                     float avg = p_in->mean();

                     msg += "- addr: " + ptr_as_string((void *)(p_in)) + "\n";
                     msg += "- min: " + std::to_string(min) + "\n";
                     msg += "- max: " + std::to_string(max) + "\n";
                     msg += "- avg: " + std::to_string(avg) + "\n";
                     msg += "- shape.x: " + std::to_string(p_in->shape.x) + "\n";
                     msg += "- shape.y: " + std::to_string(p_in->shape.y) + "\n";
                     msg += "- tiling.x: " + std::to_string(p_in->tiling.x) + "\n";
                     msg += "- tiling.y: " + std::to_string(p_in->tiling.y) + "\n";
                     msg += "- overlap: " + std::to_string(p_in->overlap) + "\n";
                   }
                   else
                   {
                     msg += "addr: nullptr\n";
                   }

                   label->setText(msg.c_str());
                   label->adjustSize();
                 });

    return (QWidget *)label;
  };

  node.set_qwidget_fct(lambda);
}

void compute_debug_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // nothing here  on purpose

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
