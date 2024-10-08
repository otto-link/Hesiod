/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <set>

#include <QCheckBox>
#include <QComboBox>

#include "highmap/colorize.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/heightmap_rgba_data.hpp"
#include "hesiod/gui/viewer2d_widget.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/graph_model_addon.hpp"

namespace hesiod
{

Viewer2dWidget::Viewer2dWidget(ModelConfig                    *p_config,
                               QtNodes::DataFlowGraphicsScene *p_scene,
                               QWidget                        *parent,
                               std::string                     label)
    : p_config(p_config), p_scene(p_scene), parent(parent)
{
  this->p_model = (HsdDataFlowGraphModel *)&this->p_scene->graphModel();

  // update connection
  QObject::connect(this->p_scene,
                   &QtNodes::DataFlowGraphicsScene::nodeSelected,
                   this,
                   &hesiod::Viewer2dWidget::on_node_selected);

  QObject::connect(this->p_model,
                   &hesiod::HsdDataFlowGraphModel::nodeDeleted,
                   this,
                   &hesiod::Viewer2dWidget::on_node_deleted);

  QObject::connect(this->p_model,
                   &HsdDataFlowGraphModel::computingFinished,
                   this,
                   &hesiod::Viewer2dWidget::update_after_computing);

  QObject::connect(this,
                   &hesiod::Viewer2dWidget::resized,
                   this,
                   [this](int /*width*/, int /*height*/) { this->update_label_image(); });

  // --- build up layout
  this->setWindowTitle("Viewer 2D");

  QGridLayout *layout = new QGridLayout(this);
  int          row = 0;

  // title
  if (label != "")
  {
    QLabel *widget = new QLabel(label.c_str());
    QFont   f = widget->font();
    f.setBold(true);
    widget->setFont(f);
    layout->addWidget(widget, row, 0);
    row++;
  }

  // pin this node
  {
    this->checkbox_pin_node = new QCheckBox("Pin current node");
    this->checkbox_pin_node->setChecked(Qt::Unchecked);
    layout->addWidget(this->checkbox_pin_node, row, 0, 1, 2);
    row++;
  }

  // colormap choice
  {
    this->combobox_cmap = new QComboBox();
    QStringList items;
    for (auto &[key, dummy] : cmap_map)
      this->combobox_cmap->addItem(key.c_str());
    this->combobox_cmap->setCurrentText("magma");
    layout->addWidget(this->combobox_cmap, row, 0);

    connect(this->combobox_cmap,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &Viewer2dWidget::update_label_image);
  }

  // hillshading
  {
    this->checkbox_hillshade = new QCheckBox("Hillshading");
    this->checkbox_hillshade->setChecked(false);
    layout->addWidget(this->checkbox_hillshade, row, 1);

    connect(this->checkbox_hillshade,
            &QCheckBox::stateChanged,
            this,
            &Viewer2dWidget::update_label_image);
    row++;
  }

  // image itself
  {
    this->label_image = new QLabel();
    this->label_image->setMaximumSize(256, 256);
    layout->addWidget(this->label_image, row, 0, 1, 2);
    row++;
  }

  this->setLayout(layout);
}

void Viewer2dWidget::on_node_deleted(QtNodes::NodeId const node_id)
{
  if (this->current_node_id == node_id)
    this->reset();
}

void Viewer2dWidget::on_node_selected(QtNodes::NodeId const node_id)
{
  if (node_id != this->current_node_id)
    this->update_viewport(node_id);
}

void Viewer2dWidget::reset()
{
  this->current_node_id = std::numeric_limits<uint>::max();
  this->checkbox_pin_node->setChecked(false);
  this->p_data = nullptr;
  this->update_label_image();
};

void Viewer2dWidget::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  Q_EMIT this->resized(event->size().width(), event->size().height());
}

void Viewer2dWidget::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  this->update_viewport();
}

void Viewer2dWidget::update_after_computing(QtNodes::NodeId const node_id)
{
  if (node_id == this->current_node_id)
    this->update_label_image();
}

void Viewer2dWidget::update_label_image()
{
  if (!this->p_data)
  {
    this->label_image->setText("No preview");
    return;
  }

  // check that the current node has not been deleted since, and that
  // data are indeed available

  if (this->p_model->allNodeIds().contains(this->current_node_id))
  {
    QSize           size = this->size();
    int             dmin = (int)std::min(1.f * size.rheight(), 1.f * size.rwidth());
    hmap::Vec2<int> shape = {dmin, dmin};

    // retrieve colormap choice from combo box
    hmap::Cmap cmap = (hmap::Cmap)cmap_map.at(
        this->combobox_cmap->currentText().toStdString());

    bool hs = this->checkbox_hillshade->isChecked();

    // render image
    if (this->p_data->type().id.compare("HeightMapData") == 0)
    {
      HeightMapData   *p_hdata = static_cast<HeightMapData *>(this->p_data);
      hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());

      hmap::Array array = p_h->to_array();

      std::vector<uint8_t> img = hmap::colorize(array, array.min(), array.max(), cmap, hs)
                                     .to_img_8bit();

      QImage preview_image = QImage(img.data(),
                                    array.shape.x,
                                    array.shape.y,
                                    QImage::Format_RGB888);

      QPixmap pixmap = QPixmap::fromImage(preview_image)
                           .QPixmap::scaled(shape.x, shape.y);

      this->label_image->setPixmap(pixmap);
    }
    //
    else if (this->p_data->type().id.compare("HeightMapRGBAData") == 0)
    {
      HeightMapRGBAData   *p_hdata = static_cast<HeightMapRGBAData *>(this->p_data);
      hmap::HeightMapRGBA *p_h = static_cast<hmap::HeightMapRGBA *>(p_hdata->get_ref());

      std::vector<uint8_t> img = p_h->to_img_8bit(p_h->shape);

      QImage preview_image = QImage(img.data(),
                                    p_h->shape.x,
                                    p_h->shape.y,
                                    QImage::Format_RGBA8888);

      QPixmap pixmap = QPixmap::fromImage(preview_image)
                           .QPixmap::scaled(shape.x, shape.y);

      this->label_image->setPixmap(pixmap);
    }
    //
    else
      this->label_image->setText("No preview");
  }
}

void Viewer2dWidget::update_viewport(QtNodes::NodeId const node_id)
{
  if (!this->checkbox_pin_node->isChecked())
    this->current_node_id = node_id;

  if (this->p_model->allNodeIds().contains(this->current_node_id))
  {
    hesiod::BaseNode *p_node = this->p_model->delegateModel<hesiod::BaseNode>(
        this->current_node_id);
    this->p_data = p_node->get_viewer2d_data();
  }
  else
    this->p_data = nullptr;

  if (this->isVisible())
    this->update_label_image();
}

void Viewer2dWidget::update_viewport() { this->update_viewport(this->current_node_id); }

} // namespace hesiod
