/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QPushButton>

#include "doubleslider.hpp" // from external/Attributes

#include "hesiod/gui/widgets/open_gl/open_gl_hmm_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

OpenGLHmmWidget::OpenGLHmmWidget(QWidget *parent) : QWidget(parent)
{
  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  this->renderer = new OpenGLHmmRender(parent);
  layout->addWidget(this->renderer, 0, 0, 1, 2);

  // h scale
  {
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

  // wireframe
  {
    std::string label = this->renderer->get_wireframe_mode() ? "Wireframe" : "Solid";

    QPushButton *button = new QPushButton(label.c_str());
    button->setCheckable(true);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(button, 1, 1);
    this->connect(button,
                  &QPushButton::toggled,
                  [this, button]()
                  {
                    this->renderer->set_wireframe_mode(
                        !this->renderer->get_wireframe_mode());

                    if (this->renderer->get_wireframe_mode())
                      button->setText("Wireframe");
                    else
                      button->setText("Solid");
                  });
  }
}

void OpenGLHmmWidget::set_data(BaseNode          *new_p_node,
                               const std::string &new_port_id_elev,
                               const std::string &new_port_id_color)
{
  // pass through the renderer
  this->renderer->set_data(new_p_node, new_port_id_elev, new_port_id_color);
}

} // namespace hesiod
