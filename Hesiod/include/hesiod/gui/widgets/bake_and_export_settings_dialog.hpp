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

struct BakeSettings
{
  int  resolution = 1024;
  int  nvariants = 0;
  bool force_distributed = true;
  bool force_auto_export = true;
  bool rename_export_files = true;
};

class BakeAndExportSettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit BakeAndExportSettingsDialog(int                 max_size,
                                       const BakeSettings &initial_value,
                                       QWidget            *parent = nullptr)
      : QDialog(parent)
  {
    this->setWindowTitle("Bake and export settings");
    this->setModal(true);

    this->resolution_combo = new QComboBox(this);
    this->slider = new QSlider(Qt::Horizontal, this);
    this->slider_nvariants = new QSpinBox(this);
    this->checkbox_force_distributed = new QCheckBox("Force distributed computation",
                                                     this);
    this->checkbox_force_auto_export = new QCheckBox("Force auto export for export nodes",
                                                     this);
    this->checkbox_rename_export_files = new QCheckBox("Add prefix export filenames",
                                                       this);

    this->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                         this);

    // Fill power-of-two options up to max_size
    for (int size = 2; size <= max_size; size *= 2)
    {
      this->resolution_combo->addItem(QString("%1 x %1").arg(size), size);
    }
    this->resolution_combo->setCurrentIndex(
        this->resolution_combo->findData(initial_value.resolution));

    // Setup slider + spinbox
    this->slider->setRange(0, 50);
    this->slider_nvariants->setRange(0, 50);
    this->slider_nvariants->setValue(initial_value.nvariants);
    this->slider->setValue(initial_value.nvariants);

    QObject::connect(this->slider,
                     &QSlider::valueChanged,
                     this->slider_nvariants,
                     &QSpinBox::setValue);
    QObject::connect(this->slider_nvariants,
                     QOverload<int>::of(&QSpinBox::valueChanged),
                     this->slider,
                     &QSlider::setValue);

    // Checkbox default state (input parameter)
    this->checkbox_force_distributed->setChecked(initial_value.force_distributed);
    this->checkbox_force_auto_export->setChecked(initial_value.force_auto_export);
    this->checkbox_rename_export_files->setChecked(initial_value.rename_export_files);

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
    main_layout->addWidget(this->checkbox_force_auto_export);
    main_layout->addWidget(this->checkbox_rename_export_files);
    main_layout->addWidget(this->buttons);
    this->setLayout(main_layout);

    QObject::connect(this->buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(this->buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  }

  BakeSettings get_bake_settings() const
  {
    return BakeSettings(this->resolution_combo->currentData().toInt(),
                        this->slider_nvariants->value(),
                        this->checkbox_force_distributed->isChecked(),
                        this->checkbox_force_auto_export->isChecked(),
                        this->checkbox_rename_export_files->isChecked());
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
  QCheckBox        *checkbox_force_auto_export;
  QCheckBox        *checkbox_rename_export_files;
  QDialogButtonBox *buttons;
};

} // namespace hesiod
