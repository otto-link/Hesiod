/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>

#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/widgets/graph_config_dialog.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

GraphConfigDialog::GraphConfigDialog(GraphConfig *p_model_config, QWidget *parent)
    : QDialog(parent), p_model_config(p_model_config)
{
  this->setWindowTitle("Hesiod - Model configuration");

  QGridLayout *layout = new QGridLayout(this);

  int row = 0;

  // --- shape
  QLabel *label_shape_text = new QLabel("shape", this);
  layout->addWidget(label_shape_text, row, 0);

  this->slider_shape = new QSlider(Qt::Horizontal, this);
  this->slider_shape->setRange(8, 12); // 256 -> 4096
  this->slider_shape->setSingleStep(1);
  this->slider_shape->setPageStep(1);
  int pos = static_cast<int>(std::log2(this->p_model_config->shape.x));
  this->slider_shape->setValue(pos);
  layout->addWidget(this->slider_shape, row, 1);

  this->label_shape = new QLabel(QString("%1x%2")
                                     .arg(this->p_model_config->shape.x)
                                     .arg(this->p_model_config->shape.y),
                                 this);
  layout->addWidget(this->label_shape, row, 2);

  this->connect(this->slider_shape,
                &QSlider::valueChanged,
                [this]()
                {
                  int pos = this->slider_shape->value();
                  this->p_model_config->set_shape(
                      hmap::Vec2<int>(std::pow(2, pos), std::pow(2, pos)));
                  this->label_shape->setText(QString("%1x%2")
                                                 .arg(this->p_model_config->shape.x)
                                                 .arg(this->p_model_config->shape.y));
                });

  row++;

  // --- tiling
  QLabel *label_tiling_text = new QLabel("tiling", this);
  layout->addWidget(label_tiling_text, row, 0);

  this->slider_tiling = new QSlider(Qt::Horizontal, this);

  // slider works on exponents instead of actual values
  int min_exp = 1; // 2^1 = 2
  int max_exp = 4; // 2^4 = 16
  this->slider_tiling->setRange(min_exp, max_exp);
  this->slider_tiling->setSingleStep(1);
  this->slider_tiling->setPageStep(1);

  // initialize slider to current tiling.x (must be power of two)
  int current_exp = std::log2(this->p_model_config->tiling.x);
  this->slider_tiling->setValue(current_exp);

  layout->addWidget(this->slider_tiling, row, 1);

  this->label_tiling = new QLabel(QString("%1x%2")
                                      .arg(this->p_model_config->tiling.x)
                                      .arg(this->p_model_config->tiling.y),
                                  this);
  layout->addWidget(this->label_tiling, row, 2);

  this->connect(this->slider_tiling,
                &QSlider::valueChanged,
                [this](int exp)
                {
                  int val = 1 << exp; // 2^exp
                  this->p_model_config->tiling.x = val;
                  this->p_model_config->tiling.y = val;
                  this->label_tiling->setText(QString("%1x%2")
                                                  .arg(this->p_model_config->tiling.x)
                                                  .arg(this->p_model_config->tiling.y));
                });

  row++;

  // --- overlap
  QLabel *label_overlap_text = new QLabel("overlap", this);
  layout->addWidget(label_overlap_text, row, 0);

  this->slider_overlap = new QSlider(Qt::Horizontal, this);
  this->slider_overlap->setRange(0, this->steps);
  this->slider_overlap->setSingleStep(1);
  this->slider_overlap->setPageStep(1);
  pos = float_to_slider_pos(this->p_model_config->overlap,
                            this->vmin,
                            this->vmax,
                            this->steps);
  this->slider_overlap->setValue(pos);
  layout->addWidget(this->slider_overlap, row, 1);

  this->label_overlap = new QLabel(QString::number(this->p_model_config->overlap, 'f', 2),
                                   this);
  layout->addWidget(this->label_overlap, row, 2);

  this->connect(this->slider_overlap,
                &QSlider::valueChanged,
                [this]()
                {
                  float v = slider_pos_to_float(this->slider_overlap->value(),
                                                this->vmin,
                                                this->vmax,
                                                this->steps);
                  this->label_overlap->setText(QString::number(v, 'f', 2));
                  this->p_model_config->overlap = v;
                });

  row++;

  // --- transform modes
  auto add_transform_combobox =
      [this, layout, &row](const std::string &label_text, hmap::TransformMode &mode)
  {
    QLabel *label = new QLabel(label_text.c_str(), this);
    layout->addWidget(label, row, 0);

    QComboBox *combobox = new QComboBox(this);
    for (auto &[name, id] : hmap::transform_mode_as_string)
    {
      combobox->addItem(QString::fromStdString(name));
      if (id == static_cast<int>(mode))
        combobox->setCurrentText(QString::fromStdString(name));
    }

    this->connect(combobox,
                  QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [this, combobox, &mode]()
                  {
                    std::string current_choice = combobox->currentText().toStdString();
                    Logger::log()->trace("Selected transform mode: {}", current_choice);
                    mode = static_cast<hmap::TransformMode>(
                        hmap::transform_mode_as_string.at(current_choice));
                  });

    layout->addWidget(combobox, row, 1, 1, 3);
    row++;
  };

  add_transform_combobox("CPU", this->p_model_config->hmap_transform_mode_cpu);
  add_transform_combobox("GPU", this->p_model_config->hmap_transform_mode_gpu);

  // --- buttons
  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                          QDialogButtonBox::Cancel,
                                                      this);
  layout->addWidget(button_box, row, 0, 1, 3);

  this->connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
  this->connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

  this->setLayout(layout);
}

} // namespace hesiod
