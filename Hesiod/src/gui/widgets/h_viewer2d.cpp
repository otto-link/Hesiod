/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "macrologger.h"

#include "hesiod/data/heightmap_data.hpp"
#include "hesiod/gui/h_viewer2d.hpp"
// #include "hesiod/data/mask_data.hpp"
#include "hesiod/model/base_node.hpp"

namespace hesiod
{

HViewer2d::HViewer2d(ModelConfig *p_config, QtNodes::DataFlowGraphModel *p_model)
    : p_config(p_config), p_model(p_model)
{
  this->setMinimumSize(512, 512); // TODO
  this->label = new QLabel(this);
  this->label->resize(512, 512);
}

void HViewer2d::update_viewport(QtNodes::NodeId const node_id)
{
  LOG_DEBUG("viewer2d, node [%d]", (int)node_id);

  hesiod::BaseNode  *p_node = this->p_model->delegateModel<hesiod::BaseNode>(node_id);
  QtNodes::NodeData *p_data = p_node->p_viewer2d_data;

  if (p_data)
  {
    QSize           size = this->size();
    int             dmin = std::min(size.rheight(), size.rwidth());
    hmap::Vec2<int> shape = {dmin, dmin};

    if (p_data->type().id.compare("HeightMapData") == 0)
    {
      HeightMapData   *p_hdata = static_cast<HeightMapData *>(p_data);
      hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(p_hdata->get_ref());
      hmap::Array      array = p_h->to_array(shape);

      std::vector<uint8_t> img =
          hmap::colorize(array, array.min(), array.max(), hmap::cmap::magma, false);

      QImage preview_image =
          QImage(img.data(), array.shape.x, array.shape.y, QImage::Format_RGB888);

      this->label->setPixmap(QPixmap::fromImage(preview_image));
    }
    else
      this->label->setText("No preview");
  }
  else
    this->label->setText("No preview");
}

} // namespace hesiod
