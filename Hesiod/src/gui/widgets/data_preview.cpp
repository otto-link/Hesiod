/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>
#include <typeinfo>

#include <QMenu>
#include <QPainter>

#include "gnodegui/style.hpp"

#include "highmap/colorize.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/tensor.hpp"

#include "hesiod/config.hpp"
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

  this->setFixedSize(
      QSize(HSD_CONFIG->nodes.shape_preview.x, HSD_CONFIG->nodes.shape_preview.y));

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

  this->update_preview();
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
        this->update_preview();
        return;
      }

    // check next inputs and outputs
    for (int k = 0; k < this->p_proxy_node->get_nports(); k++)
      if (this->p_proxy_node->get_port_caption(k) == action_label)
      {
        this->preview_port_index = k;
        this->update_preview();
        return;
      }
  }
}

const QPixmap &DataPreview::get_preview_pixmap() const { return this->preview_pixmap; }

void DataPreview::update_preview()
{
  // Retrieve port informations
  void       *blind_data_ptr = this->p_proxy_node->get_data_ref(this->preview_port_index);
  std::string data_type = this->p_proxy_node->get_data_type(this->preview_port_index);

  hmap::Vec2<int> shape_preview = HSD_CONFIG->nodes.shape_preview;
  this->resize(shape_preview.x, shape_preview.y);

  LOG->trace("DataPreview::update_preview, data_type: {}", data_type);

  // Preview image (transparent by default if no data or no rendering
  // for the requested data type)
  QImage::Format       img_format = QImage::Format_Grayscale8;
  std::vector<uint8_t> img(shape_preview.x * shape_preview.y);

  // Update preview
  if (blind_data_ptr)
  {
    if (data_type == typeid(hmap::Heightmap).name() ||
        data_type == typeid(hmap::Array).name())
    {
      hmap::Array array;

      if (data_type == typeid(hmap::Heightmap).name())
      {
        hmap::Heightmap *p_h = static_cast<hmap::Heightmap *>(blind_data_ptr);

        if (p_h)
          array = p_h->to_array(shape_preview);
        else
          LOG->error("DataPreview::update_preview: hmap::Heightmap is nullptr");
      }
      else
      {
        hmap::Array *p_a = static_cast<hmap::Array *>(blind_data_ptr);

        if (p_a)
        {
          array = *p_a;
          array = array.resample_to_shape_nearest(shape_preview);
        }
        else
        {
          LOG->error("DataPreview::update_preview: hmap::Array is nullptr");
        }
      }

      // build preview
      if (this->preview_type == PreviewType::GRAYSCALE)
      {
        img = hmap::colorize_grayscale(array).to_img_8bit();
        img_format = QImage::Format_Grayscale8;
      }
      else if (this->preview_type == PreviewType::MAGMA)
      {
        img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, true)
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
    else if (data_type == typeid(hmap::HeightmapRGBA).name())
    {
      hmap::HeightmapRGBA *p_h = static_cast<hmap::HeightmapRGBA *>(blind_data_ptr);

      if (p_h)
      {
        img = p_h->to_img_8bit(shape_preview);
        img_format = QImage::Format_RGBA8888;
      }
      else
      {
        LOG->error("DataPreview::update_preview: hmap::HeightmapRGBA is nullptr");
      }
    }
    //
    else if (data_type == typeid(hmap::Cloud).name())
    {
      hmap::Cloud *p_cloud = static_cast<hmap::Cloud *>(blind_data_ptr);

      if (p_cloud)
      {
        hmap::Cloud cloud = *p_cloud;

        if (cloud.get_npoints() > 0)
        {
          hmap::Array array = hmap::Array(shape_preview);
          cloud.to_array(array);

          img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
                    .to_img_8bit();
          img_format = QImage::Format_RGB888;
        }
      }
      else
      {
        LOG->error("DataPreview::update_preview: hmap::Cloud is nullptr");
      }
    }
    //
    else if (data_type == typeid(hmap::Path).name())
    {
      hmap::Path *p_path = static_cast<hmap::Path *>(blind_data_ptr);

      if (p_path)
      {
        hmap::Path path = *p_path;

        if (path.get_npoints() > 0)
        {
          hmap::Array       array = hmap::Array(shape_preview);
          hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
          path.remap_values(0.1f, 1.f);
          path.to_array(array, bbox);

          img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
                    .to_img_8bit();
          img_format = QImage::Format_RGB888;
        }
      }
      else
      {
        LOG->error("DataPreview::update_preview: hmap::Path is nullptr");
      }
    }
  }

  // temporary image
  QImage image;

  if (!img.empty())
  {
    // check size
    size_t nchannels = 1;

    switch (img_format)
    {
    case QImage::Format_Grayscale8:
      nchannels = 1;
      break;
    case QImage::Format_RGB888:
      nchannels = 3;
      break;
    case QImage::Format_RGBA8888:
      nchannels = 4;
      break;
    default:
      LOG->critical("DataPreview::update_preview: unknown image format");
      throw std::runtime_error("Unknown image format");
    }

    if (img.size() != shape_preview.x * shape_preview.y * nchannels)
    {
      LOG->critical("DataPreview::update_preview: image data does not fit the QImage "
                    "preview format");
      throw std::runtime_error("Wrong image data");
    }

    // eventually generate the image
    image = QImage(img.data(), shape_preview.x, shape_preview.y, img_format);
  }
  else
  {
    image = QImage(shape_preview.x, shape_preview.y, img_format);
    image.fill(Qt::transparent);
  }

  this->preview_pixmap = QPixmap::fromImage(image);

  // add a border
  QPainter painter(&this->preview_pixmap);
  QPen     pen(QColor(0, 0, 0));
  pen.setWidth(1);
  painter.setPen(pen);
  painter.drawRect(0,
                   0,
                   this->preview_pixmap.width() - 1,
                   this->preview_pixmap.height() - 1);
  painter.end();

  this->setPixmap(this->preview_pixmap);
  this->update();
}

} // namespace hesiod
