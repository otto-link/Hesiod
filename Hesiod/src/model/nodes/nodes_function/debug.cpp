/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <sstream>

#include <QLabel>

#include "highmap/heightmap.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void setup_debug_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");

  // no attribute(s)

  // specialized GUI
  auto lambda = [](BaseNode *p_node)
  {
    QLabel *label = new QLabel("DEBUG", p_node);
    label->setAlignment(Qt::AlignLeft);

    p_node->connect(p_node,
                    &BaseNode::compute_finished,
                    [label, p_node]()
                    {
                      hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>(
                          "input");

                      std::string msg = "DEBUG\n";

                      if (p_in)
                      {
                        std::ostringstream oss;
                        oss << p_in;
                        std::string ptr_as_str = oss.str();

                        float min = p_in->min();
                        float max = p_in->max();
                        float avg = p_in->mean();

                        msg += "- addr: " + ptr_as_str + "\n";
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
  p_node->set_qwidget_fct(lambda);
}

void compute_debug_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // nothing here  on purpose

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
