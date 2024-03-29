/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <set>

#include <QCheckBox>
#include <QComboBox>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "macrologger.h"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/mask_data.hpp"
#include "hesiod/gui/h_viewer2d.hpp"
#include "hesiod/model/enum_mapping.hpp"

namespace hesiod
{

HViewer2d::HViewer2d(ModelConfig                    *p_config,
                     QtNodes::DataFlowGraphicsScene *p_scene,
                     QWidget                        *parent)
    : p_config(p_config), p_scene(p_scene), parent(parent)
{
  this->p_model = (QtNodes::DataFlowGraphModel *)&this->p_scene->graphModel();

  // update connection
  QObject::connect(p_scene,
                   &QtNodes::DataFlowGraphicsScene::nodeSelected,
                   this,
                   &hesiod::HViewer2d::update_viewport);

  QObject::connect(this,
                   &hesiod::HViewer2d::resized,
                   this,
                   [this](int /*width*/, int /*height*/) { this->update_label_image(); });

  // --- build up layout
  this->setWindowTitle("Viewer 2D");

  this->layout = new QVBoxLayout(this);
  this->layout->setSpacing(0);
  this->layout->setContentsMargins(0, 0, 0, 0);

  // colormap choice
  {
    this->combobox_cmap = new QComboBox();
    QStringList items;
    for (auto &[key, dummy] : cmap_map)
      this->combobox_cmap->addItem(key.c_str());
    this->combobox_cmap->setCurrentText("magma");
    layout->addWidget(this->combobox_cmap);

    connect(this->combobox_cmap,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &HViewer2d::update_label_image);
  }

  // hillshading
  {
    this->checkbox_hillshade = new QCheckBox("Hillshading");
    this->checkbox_hillshade->setChecked(false);
    layout->addWidget(this->checkbox_hillshade);

    connect(this->checkbox_hillshade,
            &QCheckBox::stateChanged,
            this,
            &HViewer2d::update_label_image);
  }

  // pin this node
  {
    this->checkbox_pin_node = new QCheckBox("Pin current node");
    this->checkbox_pin_node->setChecked(false);
    layout->addWidget(this->checkbox_pin_node);
  }

  // image itself
  {
    this->setMinimumSize(512, 512); // TODO
    this->label_image = new QLabel();
    this->label_image->resize(512, 512);
    layout->addWidget(this->label_image);
  }

  this->setLayout(layout);
}

void HViewer2d::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  Q_EMIT this->resized(event->size().width(), event->size().height());
}

void HViewer2d::update_label_image()
{
  // check that the current node has not been deleted since, and that
  // data are indeed available

  if (this->p_model->allNodeIds().contains(this->current_node_id) && this->p_data)
  {
    QSize           size = this->size();
    int             dmin = std::min(size.rheight(), size.rwidth());
    hmap::Vec2<int> shape = {dmin, dmin};

    // retrieve colormap choice from combo box
    hmap::cmap cmap = (hmap::cmap)cmap_map.at(
        this->combobox_cmap->currentText().toStdString());

    bool hs = this->checkbox_hillshade->isChecked();

    // render image
    if (this->p_data->type().id.compare("HeightMapData") == 0)
    {
      HeightMapData   *p_hdata = static_cast<HeightMapData *>(this->p_data);
      hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());

      // TODO use interpolation to avoid issues with array shape vs widget size
      hmap::Array array = p_h->to_array();

      std::vector<uint8_t> img = hmap::colorize(array,
                                                array.min(),
                                                array.max(),
                                                cmap,
                                                hs);

      QImage  preview_image = QImage(img.data(),
                                    array.shape.x,
                                    array.shape.y,
                                    QImage::Format_RGB888);
      QPixmap pixmap = QPixmap::fromImage(preview_image)
                           .QPixmap::scaled(shape.x, shape.y);
      this->label_image->setPixmap(pixmap);
    }
    //
    else if (this->p_data->type().id.compare("MaskData") == 0)
    {
      MaskData        *p_hdata = static_cast<MaskData *>(this->p_data);
      hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
      hmap::Array      array = p_h->to_array();

      std::vector<uint8_t> img = hmap::colorize_grayscale(array);

      QImage  preview_image = QImage(img.data(),
                                    array.shape.x,
                                    array.shape.y,
                                    QImage::Format_Grayscale8);
      QPixmap pixmap = QPixmap::fromImage(preview_image)
                           .QPixmap::scaled(shape.x, shape.y);
      this->label_image->setPixmap(pixmap);
    }
    else
      this->label_image->setText("No preview");
  }
  else
    this->label_image->setText("No preview");
}

void HViewer2d::update_viewport(QtNodes::NodeId const node_id)
{
  LOG_DEBUG("viewer2d, node [%d]", (int)node_id);

  if (!this->checkbox_pin_node->isChecked())
  {
    this->current_node_id = node_id;

    hesiod::BaseNode *p_node = this->p_model->delegateModel<hesiod::BaseNode>(node_id);
    this->p_data = p_node->p_viewer2d_data;
    this->update_label_image();
  }
}

} // namespace hesiod
