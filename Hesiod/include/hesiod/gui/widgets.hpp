/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

#include <QtNodes/NodeData>

#include "hesiod/model/attributes.hpp"
#include "hesiod/model/model_config.hpp"

#define HSD_SLIDER_STEPS 1000

namespace hesiod
{

// --- Main attributes widget

/**
 * @brief AttributesWidget clas, main attributes widget, automatically laid the widgets
 required for the setting of the attribute list

 */
class AttributesWidget : public QWidget
{
  Q_OBJECT

public:
  AttributesWidget() = default;

  AttributesWidget(std::map<std::string, std::unique_ptr<Attribute>> *p_attr_map);

Q_SIGNALS:
  void value_changed();

private:
  std::map<std::string, std::unique_ptr<Attribute>> *p_attr_map;
};

// --- Attribute-specific widgets

class BoolWidget : public QWidget
{
  Q_OBJECT

public:
  BoolWidget() = default;

  BoolWidget(BoolAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  BoolAttribute *p_attr;
  QCheckBox     *checkbox;

  void update_attribute();
};

class FilenameWidget : public QWidget
{
  Q_OBJECT

public:
  FilenameWidget() = default;

  FilenameWidget(FilenameAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  FilenameAttribute *p_attr;
  QPushButton       *button;
  QFileDialog       *file_dialog;

  void update_attribute();
};

class FloatWidget : public QWidget
{
  Q_OBJECT

public:
  FloatWidget() = default;

  FloatWidget(FloatAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  FloatAttribute *p_attr;
  QSlider        *slider;
  QLabel         *label;

  void update_attribute();
};

class IntWidget : public QWidget
{
  Q_OBJECT

public:
  IntWidget() = default;

  IntWidget(IntAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  IntAttribute *p_attr;
  QSlider      *slider;
  QLabel       *label;

  void update_attribute();
};

class MapEnumWidget : public QWidget
{
  Q_OBJECT

public:
  MapEnumWidget() = default;

  MapEnumWidget(MapEnumAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  MapEnumAttribute *p_attr;
  QComboBox        *combobox;

  void update_attribute();
};

class RangeWidget : public QWidget
{
  Q_OBJECT

public:
  RangeWidget() = default;

  RangeWidget(RangeAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  RangeAttribute *p_attr;
  QSlider        *slider_min, *slider_max;
  QLabel         *label_min, *label_max;
  float           vmin, vmax;

  void update_attribute();
};

class SeedWidget : public QWidget
{
  Q_OBJECT

public:
  SeedWidget() = default;

  SeedWidget(SeedAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  SeedAttribute *p_attr;
  QSpinBox      *spinbox;

  void update_attribute();
};

class WaveNbWidget : public QWidget
{
  Q_OBJECT

public:
  WaveNbWidget() = default;

  WaveNbWidget(WaveNbAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  WaveNbAttribute *p_attr;
  QSlider         *slider_x, *slider_y;
  QLabel          *label_x, *label_y;
  QCheckBox       *checkbox;

  void update_attribute();
};

// --- helpers

int float_to_slider_pos(float v, float min, float max, int slider_steps);

float slider_pos_to_float(int pos, float min, float max, int slider_steps);

} // namespace hesiod