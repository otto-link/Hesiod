/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QGraphicsView>
#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPlainTextEdit>
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

  AttributesWidget(std::map<std::string, std::unique_ptr<Attribute>> *p_attr_map,
                   std::vector<std::string> *p_attr_ordered_key = nullptr);

Q_SIGNALS:
  void value_changed();

private:
  std::map<std::string, std::unique_ptr<Attribute>> *p_attr_map;
  std::vector<std::string>                          *p_attr_ordered_key;
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

class CloudWidget : public QWidget
{
  Q_OBJECT

public:
  CloudWidget() = default;

  CloudWidget(CloudAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  CloudAttribute *p_attr;

  void update_attribute();
};

class ColorWidget : public QWidget
{
  Q_OBJECT

public:
  ColorWidget() = default;

  ColorWidget(ColorAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  ColorAttribute *p_attr;
  QLabel         *label;

  void update_attribute(QColor color);
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

class StringWidget : public QWidget
{
  Q_OBJECT

public:
  StringWidget() = default;

  StringWidget(StringAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  StringAttribute *p_attr;
  QPlainTextEdit  *text_edit;

  void update_attribute();
};

class VecFloatWidget : public QWidget
{
  Q_OBJECT

public:
  VecFloatWidget() = default;

  VecFloatWidget(VecFloatAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  VecFloatAttribute     *p_attr;
  std::vector<QLabel *>  labels = {};
  std::vector<QSlider *> sliders = {};

  void update_attribute();
};

class VecIntWidget : public QWidget
{
  Q_OBJECT

public:
  VecIntWidget() = default;

  VecIntWidget(VecIntAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  VecIntAttribute       *p_attr;
  std::vector<QLabel *>  labels = {};
  std::vector<QSlider *> sliders = {};

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

// --- Other widgets

class PointEditorWidget : public QGraphicsView
{
  Q_OBJECT

public:
  PointEditorWidget() = default;

  PointEditorWidget(std::vector<hmap::Point> *p_points, QWidget *parent = nullptr);

Q_SIGNALS:
  void changed();

public Q_SLOTS:
  void clear_scene();

  void update_points();

  void update_scene();

protected:
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  std::vector<hmap::Point> *p_points;
  int                       width, height;
  int                       data_key = 0;

  QGraphicsScene       *scene;
  QGraphicsView        *view;
  QPushButton          *add_button;
  QGraphicsEllipseItem *moving_point;
  QPointF               offset;

  void add_point(QPointF event_pos, float point_value = 1.f);
};

class HmapGLViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
  HmapGLViewer(ModelConfig       *p_config,
               QtNodes::NodeData *p_data,
               QtNodes::NodeData *p_color = nullptr,
               QWidget           *parent = nullptr);

  void set_data(QtNodes::NodeData *new_p_data, QtNodes::NodeData *new_p_color);

  void reset();

protected:
  void initializeGL() override;

  void resizeGL(int w, int h);

  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  // void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  ModelConfig       *p_config;
  QtNodes::NodeData *p_data;
  QtNodes::NodeData *p_color;

  std::vector<GLfloat> vertices = {};
  std::vector<GLfloat> colors = {};
  GLuint               shader_program;
  GLuint               vbo_vertices;
  GLuint               vbo_colors;

  // view parameters
  float scale = 0.7f;
  float h_scale = 0.4f;
  float alpha_x = 35.f;
  float alpha_y = -25.f;
  float delta_x = 0.f;
  float delta_y = 0.f;

  float fov = 60.0f;
  float aspect_ratio = 1.f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;

  QPointF mouse_pos_bckp;
  float   alpha_x_bckp, alpha_y_bckp, delta_x_bckp, delta_y_bckp;

  void bind_gl_buffers();
};

class ModelConfigWidget : public QDialog
{
  Q_OBJECT

public:
  ModelConfigWidget() = default;

  ModelConfigWidget(ModelConfig *p_model_config, QWidget *parent = nullptr);

private:
  ModelConfig *p_model_config;

  QSlider *slider_shape;
  QLabel  *label_shape;

  QSlider *slider_tiling;
  QLabel  *label_tiling;

  QSlider *slider_overlap;
  QLabel  *label_overlap;
  float    vmin = 0.f;
  float    vmax = 0.75f;
  int      steps = 3;
};

// --- helpers

int float_to_slider_pos(float v, float min, float max, int slider_steps);

QPointF qgraphicsitem_relative_coordinates(QGraphicsItem *graphics_item,
                                           QGraphicsView *view,
                                           QPointF       *p_global_pos = nullptr);

float slider_pos_to_float(int pos, float min, float max, int slider_steps);

} // namespace hesiod