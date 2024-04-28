/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>

#include <QtNodes/NodeData>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "macrologger.h"

#include "hesiod/data/cloud_data.hpp"
#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/heightmap_rgba_data.hpp"
#include "hesiod/gui/preview.hpp"

namespace hesiod
{

Preview::Preview(BaseNode *p_node) : p_node(p_node)
{
  this->resize(HSD_PREVIEW_SHAPE, HSD_PREVIEW_SHAPE);
  this->label = new QLabel(this);
  this->setStyleSheet("QLabel { background-color : black;}");
  this->update_image();
}

void Preview::update_image()
{
  QtNodes::NodeData *p_data = this->p_node->get_preview_data();

  if (p_data)
  {
    this->label->resize(this->p_node->p_config->shape_preview.x,
                        this->p_node->p_config->shape_preview.y);

    if (p_data->type().id.compare("CloudData") == 0)
    {
      CloudData  *p_cdata = static_cast<CloudData *>(p_data);
      hmap::Cloud cloud = *static_cast<hmap::Cloud *>(p_cdata->get_ref());
      hmap::Array array = hmap::Array(this->p_node->p_config->shape_preview);

      if (cloud.get_npoints() > 0)
      {
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
        cloud.set_values(1.f);
        cloud.to_array(array, bbox);
      }

      std::vector<uint8_t> img = hmap::colorize_grayscale(array);
      QImage               preview_image = QImage(img.data(),
                                    this->p_node->p_config->shape_preview.x,
                                    this->p_node->p_config->shape_preview.y,
                                    QImage::Format_Grayscale8);

      this->label->setPixmap(QPixmap::fromImage(preview_image));
    }
    //
    else if (p_data->type().id.compare("HeightMapData") == 0)
    {
      HeightMapData   *p_hdata = static_cast<HeightMapData *>(p_data);
      hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
      hmap::Array      array = p_h->to_array(this->p_node->p_config->shape_preview);

      switch (this->p_node->p_config->preview_type)
      {
      case (PreviewType::GRAYSCALE):
      {
        std::vector<uint8_t> img = hmap::colorize_grayscale(array);

        QImage preview_image = QImage(img.data(),
                                      this->p_node->p_config->shape_preview.x,
                                      this->p_node->p_config->shape_preview.y,
                                      QImage::Format_Grayscale8);
        this->label->setPixmap(QPixmap::fromImage(preview_image));
      }
      break;
      //
      case (PreviewType::MAGMA):
      {
        std::vector<uint8_t> img = hmap::colorize(array,
                                                  array.min(),
                                                  array.max(),
                                                  hmap::cmap::magma,
                                                  false);

        QImage preview_image = QImage(img.data(),
                                      this->p_node->p_config->shape_preview.x,
                                      this->p_node->p_config->shape_preview.y,
                                      QImage::Format_RGB888);
        this->label->setPixmap(QPixmap::fromImage(preview_image));
      }
      }
    }
    //
    else if (p_data->type().id.compare("HeightMapRGBAData") == 0)
    {
      HeightMapRGBAData   *p_hdata = static_cast<HeightMapRGBAData *>(p_data);
      hmap::HeightMapRGBA *p_h = static_cast<hmap::HeightMapRGBA *>(p_hdata->get_ref());

      std::vector<uint8_t> img = p_h->to_img_8bit(this->p_node->p_config->shape_preview);

      QImage preview_image = QImage(img.data(),
                                    this->p_node->p_config->shape_preview.x,
                                    this->p_node->p_config->shape_preview.y,
                                    QImage::Format_RGBA8888);

      this->label->setPixmap(QPixmap::fromImage(preview_image));
    }
    else
      LOG_ERROR("no preview available for the requested data type: [%s]",
                p_data->type().id.toStdString().c_str());
  }
  else
  {
    // black image if no data available
    std::vector<uint8_t> img(this->p_node->p_config->shape_preview.x *
                             this->p_node->p_config->shape_preview.y);

    QImage preview_image = QImage(img.data(),
                                  this->p_node->p_config->shape_preview.x,
                                  this->p_node->p_config->shape_preview.y,
                                  QImage::Format_Grayscale8);

    this->label->setPixmap(QPixmap::fromImage(preview_image));
  }
}

} // namespace hesiod
