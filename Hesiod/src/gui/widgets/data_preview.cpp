/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <typeinfo>

#include <QMenu>
#include <QPainter>

#include "gnodegui/style.hpp"

#include "highmap/colorize.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/tensor.hpp"

#include "hesiod/gui/widgets/data_preview.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

DataPreview::DataPreview(gngui::NodeProxy *p_proxy_node)
    : QLabel(), p_proxy_node(p_proxy_node)
{
  LOG->trace("DataPreview::DataPreview, node {}({})",
             p_proxy_node->get_caption(),
             p_proxy_node->get_id());

  int width = (int)(GN_STYLE->node.width + 2.f * GN_STYLE->node.padding -
                    2.f * GN_STYLE->node.padding_widget_width);
  this->resize(width, width);

  this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  this->setFrameStyle(QFrame::NoFrame);
  this->setStyleSheet("QLabel { background-color : gray; }");

  // By default use first output or first input if there are no output
  this->preview_port_index = 0;
  for (int k = 0; k < this->p_proxy_node->get_nports(); k++)
    if (this->p_proxy_node->get_port_type(k) == gngui::PortType::OUT)
    {
      this->preview_port_index = k;
      break;
    }

  this->update_image();
}

void DataPreview::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu context_menu(this);

  context_menu.addSection("Preview type");

  for (auto &[key, value] : preview_type_map)
  {
    QAction *action = context_menu.addAction(key.c_str());

    if (value == this->preview_type)
    {
      action->setCheckable(true);
      action->setChecked(true);
    }
  }

  context_menu.addSection("Data");

  for (int k = 0; k < this->p_proxy_node->get_nports(); k++)
  {
    std::string action_label = this->p_proxy_node->get_port_caption(k);

    QAction *action = context_menu.addAction(action_label.c_str());

    if (k == this->preview_port_index)
    {
      action->setCheckable(true);
      action->setChecked(true);
    }
  }

  QAction *selected_action = context_menu.exec(event->globalPos());

  if (selected_action)
  {
    std::string action_label = selected_action->text().toStdString();

    // check preview type first
    for (auto &[key, _] : preview_type_map)
      if (key == action_label)
      {
        this->preview_type = preview_type_map.at(key);
        this->update_image();
        return;
      }

    // check next inputs and outputs
    for (int k = 0; k < this->p_proxy_node->get_nports(); k++)
      if (this->p_proxy_node->get_port_caption(k) == action_label)
      {
        this->preview_port_index = k;
        this->update_image();
        return;
      }
  }
}

void DataPreview::update_image()
{
  // Retrieve port informations
  void       *blind_data_ptr = this->p_proxy_node->get_data_ref(this->preview_port_index);
  std::string data_type = this->p_proxy_node->get_data_type(this->preview_port_index);

  hmap::Vec2<int> shape_preview = hmap::Vec2<int>(128, 128);
  this->resize(shape_preview.x, shape_preview.y);

  LOG->trace("DataPreview::update_image, data_type: {}", data_type);

  // Preview image (transparent by default if no data or no rendering
  // for the requested data type)
  QImage::Format       img_format = QImage::Format_Grayscale8;
  std::vector<uint8_t> img(shape_preview.x * shape_preview.y);

  // Update preview
  if (blind_data_ptr)
  {
    if (data_type == typeid(hmap::HeightMap).name())
    {
      hmap::HeightMap *p_h = static_cast<hmap::HeightMap *>(blind_data_ptr);
      hmap::Array      array = p_h->to_array(shape_preview);

      if (this->preview_type == PreviewType::GRAYSCALE)
      {
        img = hmap::colorize_grayscale(array).to_img_8bit();
        img_format = QImage::Format_Grayscale8;
      }
      else if (this->preview_type == PreviewType::MAGMA)
      {
        img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
                  .to_img_8bit();
        img_format = QImage::Format_RGB888;
      }
      else if (this->preview_type == PreviewType::TERRAIN)
      {
        img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::TERRAIN, true)
                  .to_img_8bit();
        img_format = QImage::Format_RGB888;
      }
      else if (this->preview_type == PreviewType::HISTOGRAM)
      {
        img = hmap::colorize_histogram(array).to_img_8bit();
        img_format = QImage::Format_Grayscale8;
      }
      else if (this->preview_type == PreviewType::SLOPE_ELEVATION_HEATMAP)
      {
        img = hmap::colorize_slope_height_heatmap(array, hmap::Cmap::HOT).to_img_8bit();
        img_format = QImage::Format_RGB888;
      }
    }
    //
    else if (data_type == typeid(hmap::HeightMapRGBA).name())
    {
      hmap::HeightMapRGBA *p_h = static_cast<hmap::HeightMapRGBA *>(blind_data_ptr);

      img = p_h->to_img_8bit(shape_preview);
      img_format = QImage::Format_RGBA8888;
    }
    //
    else if (data_type == typeid(hmap::Cloud).name())
    {
      hmap::Cloud cloud = *static_cast<hmap::Cloud *>(blind_data_ptr);
      hmap::Array array = hmap::Array(shape_preview);

      if (cloud.get_npoints() > 0)
      {
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
        cloud.remap_values(0.1f, 1.f);
        cloud.to_array(array, bbox);
      }

      img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
                .to_img_8bit();
      img_format = QImage::Format_RGB888;
    }
    //
    else if (data_type == typeid(hmap::Path).name())
    {
      hmap::Path  path = *static_cast<hmap::Path *>(blind_data_ptr);
      hmap::Array array = hmap::Array(shape_preview);

      if (path.get_npoints() > 0)
      {
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
        path.remap_values(0.1f, 1.f);
        path.to_array(array, bbox);
      }

      img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
                .to_img_8bit();
      img_format = QImage::Format_RGB888;
    }
  }

  this->preview_image = QImage(img.data(), shape_preview.x, shape_preview.y, img_format);
  this->setPixmap(QPixmap::fromImage(preview_image));
  this->update();
}

} // namespace hesiod
