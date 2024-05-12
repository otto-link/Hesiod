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
#include <QListWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPainter>
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

class ColorbarWidget; // forward decl.

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

class ColorGradientWidget : public QWidget
{
  Q_OBJECT

public:
  ColorGradientWidget() = default;

  ColorGradientWidget(ColorGradientAttribute *p_attr);

public Q_SLOTS:
  void add_color();

  void on_item_double_click(QListWidgetItem *item);

  void remove_color();

Q_SIGNALS:
  void value_changed();

private:
  ColorGradientAttribute *p_attr;
  ColorbarWidget         *colorbar;
  QListWidget            *color_list;

  void update();

  void update_attribute();

  void update_color_list();

  void update_label_gradient();
};

class ColormapEnumWidget : public QWidget
{
  Q_OBJECT

public:
  ColormapEnumWidget() = default;

  ColormapEnumWidget(MapEnumAttribute *p_attr);

  void on_item_click(QListWidgetItem *item);

Q_SIGNALS:
  void value_changed();

private:
  MapEnumAttribute        *p_attr;
  QListWidget             *cmap_list;
  int                      current_choice_item_idx = 0;
  std::vector<std::string> item_choice = {};
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

class PathWidget : public QWidget
{
  Q_OBJECT

public:
  PathWidget() = default;

  PathWidget(PathAttribute *p_attr);

Q_SIGNALS:
  void value_changed();

private:
  PathAttribute *p_attr;

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

class ColorbarWidget : public QLabel
{
public:
  ColorbarWidget(QWidget *parent = nullptr);

  ColorbarWidget(std::vector<std::vector<float>> colors,
                 QString                         caption = "",
                 QWidget                        *parent = nullptr);

  QString get_caption() { return this->caption; };

  std::vector<std::vector<float>> get_colors() { return this->colors; };

  void set_border(int width, QColor color);

  void set_caption(QString new_caption);

  void update_colors(std::vector<std::vector<float>> new_colors);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  std::vector<std::vector<float>> colors;
  QString                         caption = "";
  int                             border_width = 0;
  QColor                          border_color = Qt::white;
};

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
  bool render_texture = true;
  bool render_mask = true;

  HmapGLViewer(ModelConfig       *p_config,
               QtNodes::NodeData *p_data,
               QtNodes::NodeData *p_color = nullptr,
               QWidget           *parent = nullptr);

  void reset();

  void set_data_forced();

  void set_data(QtNodes::NodeData *new_p_data, QtNodes::NodeData *new_p_color);

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
  std::vector<uint>    indices = {};
  std::vector<uint8_t> texture_img = {};
  hmap::Vec2<int>      texture_shape = hmap::Vec2<int>(0, 0);

  QOpenGLVertexArrayObject qvao;
  QOpenGLShaderProgram     shader;
  GLuint                   vbo;
  GLuint                   ebo;
  GLuint                   texture_id;

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