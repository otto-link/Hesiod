/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
 * License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

namespace hesiod
{

class BakeAndExportSettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit BakeAndExportSettingsDialog(int      max_size = 4096,
                                       int      default_resolution = 512,
                                       int      default_nvariants = 0,
                                       bool     default_force_distributed = true,
                                       QWidget *parent = nullptr)
      : QDialog(parent)
  {
    this->setWindowTitle("Bake and export settings");
    this->setModal(true);

    this->resolution_combo = new QComboBox(this);
    this->slider = new QSlider(Qt::Horizontal, this);
    this->slider_nvariants = new QSpinBox(this);
    this->checkbox_force_distributed = new QCheckBox("Force distributed computation",
                                                     this);
    this->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                         this);

    // Fill power-of-two options up to max_size
    for (int size = 2; size <= max_size; size *= 2)
    {
      this->resolution_combo->addItem(QString("%1 x %1").arg(size), size);
    }
    this->resolution_combo->setCurrentIndex(
        this->resolution_combo->findData(default_resolution));

    // Setup slider + spinbox
    this->slider->setRange(0, 50);
    this->slider_nvariants->setRange(0, 50);
    this->slider_nvariants->setValue(default_nvariants);
    this->slider->setValue(default_nvariants);

    QObject::connect(this->slider,
                     &QSlider::valueChanged,
                     this->slider_nvariants,
                     &QSpinBox::setValue);
    QObject::connect(this->slider_nvariants,
                     QOverload<int>::of(&QSpinBox::valueChanged),
                     this->slider,
                     &QSlider::setValue);

    // Checkbox default state (input parameter)
    this->checkbox_force_distributed->setChecked(default_force_distributed);

    // Layout
    auto *form_layout = new QFormLayout;
    form_layout->addRow("Resolution:", this->resolution_combo);

    auto *slider_layout = new QHBoxLayout;
    slider_layout->addWidget(new QLabel("Variants:"));
    slider_layout->addWidget(this->slider);
    slider_layout->addWidget(this->slider_nvariants);
    form_layout->addRow(slider_layout);

    auto *main_layout = new QVBoxLayout;
    main_layout->addLayout(form_layout);
    main_layout->addWidget(this->checkbox_force_distributed);
    main_layout->addWidget(this->buttons);
    this->setLayout(main_layout);

    QObject::connect(this->buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(this->buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  }

  int  get_size() const { return this->resolution_combo->currentData().toInt(); }
  int  get_nvariants() const { return this->slider_nvariants->value(); }
  bool get_force_distributed() const
  {
    return this->checkbox_force_distributed->isChecked();
  }

private:
  QComboBox        *resolution_combo;
  QSlider          *slider;
  QSpinBox         *slider_nvariants;
  QCheckBox        *checkbox_force_distributed;
  QDialogButtonBox *buttons;
};

} // namespace hesiod
