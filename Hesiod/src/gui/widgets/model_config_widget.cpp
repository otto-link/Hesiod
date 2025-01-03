/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>

#include "highmap/opencl/gpu_opencl.hpp"

#include "hesiod/gui/widgets/model_config_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

int float_to_slider_pos(float v, float min, float max, int slider_steps)
{
  return (int)((v - min) / (max - min) * (float)slider_steps);
}

float slider_pos_to_float(int pos, float min, float max, int slider_steps)
{
  return min + (float)pos / (float)slider_steps * (max - min);
}

ModelConfigWidget::ModelConfigWidget(ModelConfig *p_model_config, QWidget *parent)
    : QDialog(parent), p_model_config(p_model_config)
{
  QGridLayout *layout = new QGridLayout(this);

  int row = 0;

  // --- shape

  QLabel *label_shape_text = new QLabel("shape");
  layout->addWidget(label_shape_text, row, 0);

  this->slider_shape = new QSlider(Qt::Horizontal);
  this->slider_shape->setRange(8, 12); // 256 -> 4096
  this->slider_shape->setSingleStep(1);
  this->slider_shape->setPageStep(1);
  int pos = (int)std::log2(this->p_model_config->shape.x);
  this->slider_shape->setValue(pos);
  layout->addWidget(this->slider_shape, row, 1);

  this->label_shape = new QLabel(QString().asprintf("%dx%d",
                                                    this->p_model_config->shape.x,
                                                    this->p_model_config->shape.y));
  layout->addWidget(this->label_shape, row, 2);

  connect(this->slider_shape,
          &QSlider::valueChanged,
          [this]()
          {
            int pos = this->slider_shape->value();
            this->p_model_config->set_shape(
                hmap::Vec2<int>(std::pow(2, pos), std::pow(2, pos)));
            this->label_shape->setText(QString().asprintf("%dx%d",
                                                          this->p_model_config->shape.x,
                                                          this->p_model_config->shape.y));
          });

  row++;

  // --- tiling

  QLabel *label_tiling_text = new QLabel("tiling");
  layout->addWidget(label_tiling_text, row, 0);

  this->slider_tiling = new QSlider(Qt::Horizontal);
  this->slider_tiling->setRange(1, 8);
  this->slider_tiling->setSingleStep(1);
  this->slider_tiling->setPageStep(1);
  this->slider_tiling->setValue(this->p_model_config->tiling.x);
  layout->addWidget(this->slider_tiling, row, 1);

  this->label_tiling = new QLabel(QString().asprintf("%dx%d",
                                                     this->p_model_config->tiling.x,
                                                     this->p_model_config->tiling.y));
  layout->addWidget(this->label_tiling, row, 2);

  connect(this->slider_tiling,
          &QSlider::valueChanged,
          [this]()
          {
            int pos = this->slider_tiling->value();
            this->p_model_config->tiling.x = pos;
            this->p_model_config->tiling.y = pos;
            this->label_tiling->setText(
                QString().asprintf("%dx%d",
                                   this->p_model_config->tiling.x,
                                   this->p_model_config->tiling.y));
          });

  row++;

  // --- overlap

  QLabel *label_overlap_text = new QLabel("overlap");
  layout->addWidget(label_overlap_text, row, 0);

  this->slider_overlap = new QSlider(Qt::Horizontal);
  this->slider_overlap->setRange(0, this->steps);
  this->slider_overlap->setSingleStep(1);
  this->slider_overlap->setPageStep(1);
  pos = float_to_slider_pos(this->p_model_config->overlap,
                            this->vmin,
                            this->vmax,
                            this->steps);
  this->slider_overlap->setValue(pos);
  layout->addWidget(this->slider_overlap, row, 1);

  this->label_overlap = new QLabel(
      QString().asprintf("%.2f", this->p_model_config->overlap));
  layout->addWidget(this->label_overlap, row, 2);

  connect(this->slider_overlap,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider_overlap->value(),
                                          this->vmin,
                                          this->vmax,
                                          this->steps);
            this->label_overlap->setText(QString().asprintf("%.2f", v));
            this->p_model_config->overlap = v;
          });

  row++;

  // --- OpenCL configuration

  QLabel *label_opencl = new QLabel("Hardware acceleration (OpenCL)");
  layout->addWidget(label_opencl, row, 0);
  row++;

  // get available devices
  std::map<size_t, std::string> cl_device_map = clwrapper::DeviceManager::get_instance()
                                                    .get_available_devices();
  size_t current_device = clwrapper::DeviceManager::get_instance().get_device_id();

  // setup combobox / device
  {
    QComboBox *combobox = new QComboBox();

    QStringList items;
    for (auto &[id, name] : cl_device_map)
      combobox->addItem(name.c_str());

    combobox->setCurrentText(cl_device_map.at(current_device).c_str());

    connect(combobox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [cl_device_map, combobox]()
            {
              size_t choice_index = static_cast<size_t>(combobox->currentIndex());

              LOG->trace("{}", choice_index);
              LOG->trace("{}, {}", choice_index, cl_device_map.at(choice_index));

              if (clwrapper::DeviceManager::get_instance().set_device(choice_index))
                clwrapper::KernelManager::get_instance().build_program();
              else
                LOG->error("device selection failed");
            });

    layout->addWidget(combobox, row, 0, 1, 3);
  }
  row++;

  // setup combobox / block size
  {
    QComboBox *combobox = new QComboBox();

    QStringList items;
    for (int k = 0; k < 7; k++)
      combobox->addItem(std::to_string((int)pow(2, k)).c_str());

    // combobox->setCurrentText(cl_device_map.at(current_device).c_str());

    connect(combobox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [cl_device_map, combobox]()
            {
              int k = combobox->currentIndex();
              clwrapper::KernelManager::get_instance().set_block_size((int)pow(2, k));
            });

    layout->addWidget(combobox, row, 0, 1, 3);
  }
  row++;

  // --- buttons

  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel);

  layout->addWidget(button_box, row, 0, 1, 3);

  this->setLayout(layout);

  connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

} // namespace hesiod
