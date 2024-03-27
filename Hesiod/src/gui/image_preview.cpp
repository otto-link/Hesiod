/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLabel>

#include <QtNodes/NodeData>

#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "macrologger.h"

#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/heightmap_data.hpp"

namespace hesiod
{

ImagePreview::ImagePreview(const ModelConfig &config) : p_config(&config)
{
  this->label = new QLabel();
  this->label->setMinimumSize(HSD_PREVIEW_SHAPE, HSD_PREVIEW_SHAPE);
}

void ImagePreview::update(QtNodes::NodeData *p_data)
{
  if (p_data->type().id.compare("HeightMapData") == 0)
  {
    HeightMapData   *p_hdata = static_cast<HeightMapData *>(p_data);
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
              p_data->type().id.toStdString().c_str());
}

} // namespace hesiod
