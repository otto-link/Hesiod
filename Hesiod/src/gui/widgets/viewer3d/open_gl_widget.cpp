/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QPushButton>

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
  layout->addWidget(this->renderer, 0, 0, 1, 4);

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

  // approx. mesh
  {
    std::string label = this->renderer->get_use_approx_mesh() ? "Approx. mesh"
                                                              : "Exact mesh";

    QPushButton *button = new QPushButton(label.c_str());
    button->setCheckable(true);
    layout->addWidget(button, 1, 2);
    this->connect(button,
                  &QPushButton::toggled,
                  [this, button]()
                  {
                    this->renderer->set_use_approx_mesh(
                        !this->renderer->get_use_approx_mesh());

                    if (this->renderer->get_use_approx_mesh())
                      button->setText("Approx. mesh");
                    else
                      button->setText("Exact");
                  });
  }

  // approx. error
  {
    auto slider = new ValueSliders::DoubleSlider("Max. error", 5e-3f, 1e-5f, 1e-1f);
    layout->addWidget(slider, 1, 3);

    this->renderer->set_max_approx_error(slider->getVal());

    this->connect(slider,
                  &ValueSliders::DoubleSlider::valueChanged,
                  [slider, this]()
                  {
                    float v = slider->getVal();
                    this->renderer->set_max_approx_error(v);
                  });
  }

  for (int i = 0; i < layout->columnCount(); i++)
    layout->setColumnStretch(i, 1);
}

void OpenGLWidget::on_node_compute_finished(const std::string &id)
{
  LOG->trace("OpenGLWidget::on_node_compute_finished {}", id);

  // just a pass through...
  this->renderer->on_node_compute_finished(id);
}

void OpenGLWidget::set_data(BaseNode          *new_p_node,
                            const std::string &new_port_id_elev,
                            const std::string &new_port_id_color)
{
  // pass through the renderer
  this->renderer->set_data(new_p_node, new_port_id_elev, new_port_id_color);
}

} // namespace hesiod
