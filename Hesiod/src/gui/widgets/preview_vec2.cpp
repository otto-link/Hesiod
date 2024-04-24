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
#include "hesiod/data/mask_data.hpp"
#include "hesiod/gui/preview.hpp"

namespace hesiod
{

PreviewVec2::PreviewVec2(const std::shared_ptr<HeightMapData> *p_u,
                         const std::shared_ptr<HeightMapData> *p_v,
                         const ModelConfig                    *p_config)
    : p_u(p_u), p_v(p_v), p_config(p_config)
{
  this->resize(HSD_PREVIEW_SHAPE, HSD_PREVIEW_SHAPE);
  this->label = new QLabel(this);
  this->setStyleSheet("QLabel { background-color : black;}");
  this->update_image();
}

void PreviewVec2::update_image()
{
  hmap::HeightMap *p_hmap_u = this->p_u->get()->get_ref();
  hmap::HeightMap *p_hmap_v = this->p_v->get()->get_ref();

  this->label->resize(this->p_config->shape_preview.x, this->p_config->shape_preview.y);

  hmap::Array u = p_hmap_u->to_array(this->p_config->shape_preview);
  hmap::Array v = p_hmap_v->to_array(this->p_config->shape_preview);

  std::vector<uint8_t> img = hmap::colorize_vec2(u, v);

  QImage preview_image = QImage(img.data(),
                                this->p_config->shape_preview.x,
                                this->p_config->shape_preview.y,
                                QImage::Format_RGB888);

  this->label->setPixmap(QPixmap::fromImage(preview_image));
}

} // namespace hesiod
