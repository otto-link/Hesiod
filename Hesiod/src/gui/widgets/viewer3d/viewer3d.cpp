/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <typeinfo>

#include <QComboBox>
#include <QGridLayout>

#include "highmap/heightmap.hpp"

#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/open_gl/open_gl_widget.hpp"
#include "hesiod/gui/widgets/viewer3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"

namespace hesiod
{

Viewer3d::Viewer3d(GraphNodeWidget *p_graph_node_widget,
                   QWidget         *parent,
                   std::string      label)
    : AbstractViewer(p_graph_node_widget, parent, label)
{
  QGridLayout *layout = dynamic_cast<QGridLayout *>(this->layout());
  int          row = layout->rowCount();

  // renderer widget
  {
    this->render_widget = new OpenGLWidget();

    this->connect(this->p_graph_node_widget->get_p_graph_node(),
                  &GraphNode::compute_finished,
                  static_cast<OpenGLWidget *>(this->render_widget),
                  &OpenGLWidget::on_node_compute_finished);

    this->connect(this,
                  &Viewer3d::view_param_changed,
                  static_cast<OpenGLWidget *>(this->render_widget),
                  &OpenGLWidget::set_data);
  }

  layout->addWidget(this->render_widget, row++, 0, 1, layout->columnCount());

  // update contents
  this->update_view_param_widgets();
  this->emit_view_param_changed();
}

} // namespace hesiod
