/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

#include "doubleslider.hpp" // from external/Attributes

#include "hesiod/gui/widgets/open_gl/open_gl_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

OpenGLWidget::OpenGLWidget(QWidget *parent) : QWidget(parent)
{
  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  this->renderer = new OpenGLRender(parent);
  layout->addWidget(this->renderer, 0, 0);

  auto slider = new ValueSliders::DoubleSlider("Elevation scale", 0.4f, 0.f, 1.f);
  layout->addWidget(slider, 1, 0);

  this->renderer->set_h_scale(slider->getVal());

  this->connect(slider,
                &ValueSliders::DoubleSlider::valueChanged,
                [slider, this]()
                {
                  float v = slider->getVal();
                  this->renderer->set_h_scale(v);
                });
}

void OpenGLWidget::set_data(BaseNode          *new_p_node,
                            const std::string &new_port_id_elev,
                            const std::string &new_port_id_color)
{
  // pass through the renderer
  this->renderer->set_data(new_p_node, new_port_id_elev, new_port_id_color);
}

} // namespace hesiod
