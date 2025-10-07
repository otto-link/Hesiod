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
    : QLabel(nullptr), p_proxy_node(p_proxy_node)
{
  if (!this->p_proxy_node)
    throw std::invalid_argument("DataPreview::DataPreview: p_proxy_node is nullptr");

  Logger::log()->trace("DataPreview::DataPreview, node {}({})",
                       this->p_proxy_node->get_caption(),
                       this->p_proxy_node->get_id());

  const auto shape = HSD_CONFIG->nodes.shape_preview;

  this->setFixedSize(QSize(shape.x, shape.y));
  this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  this->setFrameStyle(QFrame::NoFrame);
  this->setStyleSheet("QLabel { background-color : gray; }");

  // Select first output, or fallback to first port
  this->preview_port_index = 0;
  for (int k = 0; k < this->p_proxy_node->get_nports(); ++k)
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

  for (auto &[label, type] : preview_type_map)
  {
    QAction *action = context_menu.addAction(QString::fromStdString(label));
    action->setCheckable(true);
    if (type == preview_type)
      action->setChecked(true);
  }

  context_menu.addSection("Data");
  for (int k = 0; k < p_proxy_node->get_nports(); ++k)
  {
    const std::string caption = p_proxy_node->get_port_caption(k);
    QAction          *action = context_menu.addAction(QString::fromStdString(caption));
    action->setCheckable(true);
    if (k == preview_port_index)
      action->setChecked(true);
  }

  if (QAction *selected = context_menu.exec(event->globalPos()))
  {
    const std::string label = selected->text().toStdString();

    // Preview type
    if (auto it = preview_type_map.find(label); it != preview_type_map.end())
    {
      preview_type = it->second;
      update_preview();
      return;
    }

    // Port selection
    for (int k = 0; k < p_proxy_node->get_nports(); ++k)
      if (p_proxy_node->get_port_caption(k) == label)
      {
        preview_port_index = k;
        update_preview();
        return;
      }
  }
}

const QPixmap &DataPreview::get_preview_pixmap() const { return this->preview_pixmap; }

void DataPreview::update_preview()
{
  if (!p_proxy_node)
  {
    Logger::log()->error("DataPreview::update_preview: p_proxy_node is nullptr");
    return;
  }

  void             *blind_ptr = p_proxy_node->get_data_ref(preview_port_index);
  const std::string data_type = p_proxy_node->get_data_type(preview_port_index);
  const auto        shape = HSD_CONFIG->nodes.shape_preview;
  this->resize(shape.x, shape.y);

  Logger::log()->trace("DataPreview::update_preview, data_type: {}", data_type);

  QImage::Format       img_format = QImage::Format_Grayscale8;
  std::vector<uint8_t> img;

  auto build_colored_array =
      [&](hmap::Array &array, hmap::Cmap cmap, bool normalize = true)
  {
    const float minv = array.min();
    const float maxv = array.max();
    return hmap::colorize(array, minv, maxv, cmap, normalize).to_img_8bit();
  };

  if (blind_ptr)
  {
    // ---- Heightmap or Array ----
    if (data_type == typeid(hmap::Heightmap).name() ||
        data_type == typeid(hmap::Array).name())
    {
      hmap::Array array;

      if (data_type == typeid(hmap::Heightmap).name())
      {
        const hmap::Heightmap *p_h = static_cast<const hmap::Heightmap *>(blind_ptr);
        if (p_h)
          array = p_h->to_array(shape);
        else
          Logger::log()->error("DataPreview::update_preview: Heightmap nullptr");
      }
      else
      {
        const hmap::Array *p_a = static_cast<const hmap::Array *>(blind_ptr);
        if (p_a)
          array = p_a->resample_to_shape_nearest(shape);
        else
          Logger::log()->error("DataPreview::update_preview: Array nullptr");
      }

      switch (preview_type)
      {
      case PreviewType::GRAYSCALE:
        img = hmap::colorize_grayscale(array).to_img_8bit();
        img_format = QImage::Format_Grayscale8;
        break;
      case PreviewType::MAGMA:
        img = build_colored_array(array, hmap::Cmap::MAGMA);
        img_format = QImage::Format_RGB888;
        break;
      case PreviewType::TERRAIN:
        img = build_colored_array(array, hmap::Cmap::TERRAIN);
        img_format = QImage::Format_RGB888;
        break;
      case PreviewType::HISTOGRAM:
        img = hmap::colorize_histogram(array).to_img_8bit();
        img_format = QImage::Format_Grayscale8;
        break;
      case PreviewType::SLOPE_ELEVATION_HEATMAP:
        img = hmap::colorize_slope_height_heatmap(array, hmap::Cmap::HOT).to_img_8bit();
        img_format = QImage::Format_RGB888;
        break;
      }
    }
    // ---- RGBA ----
    else if (data_type == typeid(hmap::HeightmapRGBA).name())
    {
      const hmap::HeightmapRGBA *p_h = static_cast<const hmap::HeightmapRGBA *>(
          blind_ptr);
      if (p_h)
      {
        img = p_h->to_img_8bit(shape);
        img_format = QImage::Format_RGBA8888;
      }
      else
        Logger::log()->error("DataPreview::update_preview: HeightmapRGBA nullptr");
    }
    // ---- Cloud ----
    else if (data_type == typeid(hmap::Cloud).name())
    {
      const hmap::Cloud *p_cloud = static_cast<const hmap::Cloud *>(blind_ptr);
      if (p_cloud && p_cloud->get_npoints() > 0)
      {
        hmap::Array array(shape);
        p_cloud->to_array(array);
        img = build_colored_array(array, hmap::Cmap::MAGMA, false);
        img_format = QImage::Format_RGB888;
      }
      else
        Logger::log()->error("DataPreview::update_preview: Cloud nullptr or empty");
    }
    // ---- Path ----
    else if (data_type == typeid(hmap::Path).name())
    {
      const hmap::Path *p_path = static_cast<const hmap::Path *>(blind_ptr);
      if (p_path && p_path->get_npoints() > 0)
      {
        hmap::Array       array(shape);
        hmap::Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);
        hmap::Path        path = *p_path;
        path.remap_values(0.1f, 1.f);
        path.to_array(array, bbox);
        img = build_colored_array(array, hmap::Cmap::MAGMA, false);
        img_format = QImage::Format_RGB888;
      }
      else
        Logger::log()->error("DataPreview::update_preview: Path nullptr or empty");
    }
  }

  // ---- Build final QImage safely (copying data) ----
  QImage image;

  if (!img.empty())
  {
    size_t nchannels = (img_format == QImage::Format_Grayscale8) ? 1
                       : (img_format == QImage::Format_RGB888)   ? 3
                                                                 : 4;

    if (img.size() != static_cast<size_t>(shape.x * shape.y * nchannels))
    {
      Logger::log()->critical(
          "DataPreview::update_preview: inconsistent image buffer size");
      throw std::runtime_error("DataPreview::update_preview: inconsistent buffer size");
    }

    // Copy buffer to QImage that owns its data
    image = QImage(shape.x, shape.y, img_format);
    std::memcpy(image.bits(), img.data(), img.size());
  }
  else
  {
    image = QImage(shape.x, shape.y, img_format);
    image.fill(Qt::transparent);
  }

  // ---- Draw border ----
  preview_pixmap = QPixmap::fromImage(image);
  {
    QPainter painter(&preview_pixmap);
    QPen     pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(0, 0, preview_pixmap.width() - 1, preview_pixmap.height() - 1);
  }

  this->setPixmap(this->preview_pixmap);
  this->update();
}

} // namespace hesiod
