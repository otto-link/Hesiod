/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>

#include "qsx/slider_float.hpp" // from external/Attributes

#include "hesiod/gui/widgets/open_gl/open_gl_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

OpenGLWidget::OpenGLWidget(QWidget *parent) : QWidget(parent)
{
  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  this->renderer = new OpenGLRender(parent);
  layout->addWidget(this->renderer, 0, 0, 1, 5);

  int col = 0;
  int row = 1;

  // h scale
  {
    auto *slider = new qsx::SliderFloat("Elevation scale",
                                        0.4f,
                                        0.f,
                                        1.f,
                                        false, // +/- buttons
                                        "{:.3f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_h_scale(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_h_scale(v);
                  });
  }

  // wireframe
  {
    std::string label = this->renderer->get_wireframe_mode() ? "Wireframe" : "Solid";

    QPushButton *button = new QPushButton(label.c_str());
    button->setCheckable(true);
    layout->addWidget(button, row, col++);
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

  // normal map
  {
    std::vector<std::string> labels = {"Texture", "Normal map", "Heightmap", "Terrain"};

    QComboBox *combobox = new QComboBox();

    for (auto &label : labels)
      combobox->addItem(label.c_str());

    layout->addWidget(combobox, row, col++);

    connect(combobox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, combobox]()
            {
              std::string current_choice = combobox->currentText().toStdString();

              this->renderer->set_show_normal_map(false);
              this->renderer->set_show_heightmap(false);
              this->renderer->set_show_terrain_cmap(false);

              if (current_choice == "Normal map")
                this->renderer->set_show_normal_map(true);
              else if (current_choice == "Heightmap")
                this->renderer->set_show_heightmap(true);
              else if (current_choice == "Terrain")
                this->renderer->set_show_terrain_cmap(true);
            });
  }

  // approx. mesh
  {
    std::string label = this->renderer->get_use_approx_mesh() ? "Approx. mesh"
                                                              : "Exact mesh";

    QPushButton *button = new QPushButton(label.c_str());
    button->setCheckable(true);
    layout->addWidget(button, row, col++);
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
    auto *slider = new qsx::SliderFloat("Max. error",
                                        5e-3f,
                                        1e-5f,
                                        1e-1f,
                                        false, // +/- buttons
                                        "{:.4f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_max_approx_error(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_max_approx_error(v);
                  });
  }

  // --- second line

  row++;
  col = 0;

  // zenith
  {
    auto *slider = new qsx::SliderFloat("Light zenith",
                                        0.f,
                                        0.f,
                                        90.f,
                                        false, // +/- buttons
                                        "{:.3f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_zenith(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_zenith(v);
                  });
  }

  // azimuth
  {
    auto *slider = new qsx::SliderFloat("Light azimuth",
                                        0.f,
                                        -180.f,
                                        180.f,
                                        false, // +/- buttons
                                        "{:.3f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_azimuth(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_azimuth(v);
                  });
  }

  // saturation
  {
    auto *slider = new qsx::SliderFloat("Shadow saturation",
                                        0.f,
                                        -1.f,
                                        1.f,
                                        false, // +/- buttons
                                        "{:.3f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_shadow_saturation(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_shadow_saturation(v);
                  });
  }

  // strength
  {
    auto *slider = new qsx::SliderFloat("Shadow strength",
                                        1.f,
                                        0.f,
                                        1.f,
                                        false, // +/- buttons
                                        "{:.3f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_shadow_strength(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_shadow_strength(v);
                  });
  }

  // gamma
  {
    auto *slider = new qsx::SliderFloat("Shadow gamma",
                                        0.7f,
                                        0.1f,
                                        2.f,
                                        false, // +/- buttons
                                        "{:.3f}");
    layout->addWidget(slider, row, col++);

    this->renderer->set_shadow_gamma(slider->get_value());

    this->connect(slider,
                  &qsx::SliderFloat::value_changed,
                  [slider, this]()
                  {
                    float v = slider->get_value();
                    this->renderer->set_shadow_gamma(v);
                  });
  }
  // --- fix column width

  for (int i = 0; i < layout->columnCount(); i++)
    layout->setColumnStretch(i, 1);
}

void OpenGLWidget::on_node_compute_finished(const std::string &graph_id,
                                            const std::string &id)
{
  LOG->trace("OpenGLWidget::on_node_compute_finished {}/{}", graph_id, id);

  // just a pass through...
  this->renderer->on_node_compute_finished(graph_id, id);
}

void OpenGLWidget::set_data(BaseNode          *new_p_node,
                            const std::string &new_port_id_elev,
                            const std::string &new_port_id_color,
                            const std::string &new_port_id_normal_map)
{
  // pass through the renderer
  this->renderer->set_data(new_p_node,
                           new_port_id_elev,
                           new_port_id_color,
                           new_port_id_normal_map);
}

} // namespace hesiod
