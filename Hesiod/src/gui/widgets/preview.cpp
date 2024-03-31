/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>

#include <QtNodes/NodeData>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "macrologger.h"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/data/mask_data.hpp"
#include "hesiod/gui/preview.hpp"

namespace hesiod
{

Preview::Preview(const ModelConfig *p_config, QtNodes::NodeData *p_data)
    : p_config(p_config), p_data(p_data)
{
  this->resize(HSD_PREVIEW_SHAPE, HSD_PREVIEW_SHAPE);
  this->label = new QLabel(this);
  this->update_image();
}

void Preview::update_image()
{
  if (this->p_data->type().id.compare("HeightMapData") == 0)
  {
    HeightMapData   *p_hdata = static_cast<HeightMapData *>(this->p_data);
    hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
    hmap::Array      array = p_h->to_array(this->p_config->shape_preview);

    std::vector<uint8_t> img = hmap::colorize_grayscale(array);

    QImage preview_image = QImage(img.data(),
                                  this->p_config->shape_preview.x,
                                  this->p_config->shape_preview.y,
                                  QImage::Format_Grayscale8);

    // std::vector<uint8_t> img = hmap::colorize(array,
    //                                           array.min(),
    //                                           array.max(),
    //                                           hmap::cmap::magma,
    //                                           false);

    // QImage preview_image = QImage(img.data(),
    //                               this->p_config->shape_preview.x,
    //                               this->p_config->shape_preview.y,
    //                               QImage::Format_RGB888);

    this->label->setPixmap(QPixmap::fromImage(preview_image));
  }
  else if (this->p_data->type().id.compare("MaskData") == 0)
  {
    MaskData        *p_hdata = static_cast<MaskData *>(this->p_data);
    hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
    hmap::Array      array = p_h->to_array(this->p_config->shape_preview);

    std::vector<uint8_t> img = hmap::colorize_grayscale(array);

    QImage preview_image = QImage(img.data(),
                                  this->p_config->shape_preview.x,
                                  this->p_config->shape_preview.y,
                                  QImage::Format_Grayscale8);

    this->label->setPixmap(QPixmap::fromImage(preview_image));
  }
  else
    LOG_ERROR("no preview available for the requested data type: [%s]",
              this->p_data->type().id.toStdString().c_str());
}

} // namespace hesiod
