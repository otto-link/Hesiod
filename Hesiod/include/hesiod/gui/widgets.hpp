/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

#include <QtNodes/NodeData>

#include "hesiod/model/attributes.hpp"
#include "hesiod/model/model_config.hpp"

#define HSD_SLIDER_STEPS 1000

namespace hesiod
{

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

// --- helpers

int float_to_slider_pos(float v, float min, float max, int slider_steps);

float slider_pos_to_float(int pos, float min, float max, int slider_steps);

} // namespace hesiod