/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QContextMenuEvent>
#include <QLabel>

#include "gnodegui/node_proxy.hpp"

#include "hesiod/logger.hpp"

namespace hesiod
{

enum PreviewType : int
{
  GRAYSCALE,
  MAGMA,
  TERRAIN,
  SLOPE_ELEVATION_HEATMAP,
  HISTOGRAM,
};

static std::map<std::string, PreviewType> preview_type_map = {
    {"cmap Grayscale", PreviewType::GRAYSCALE},
    {"cmap Magma", PreviewType::MAGMA},
    {"cmap Terrain (hillshade)", PreviewType::TERRAIN},
    {"Histogram view", PreviewType::HISTOGRAM},
    // {"Slope/elev. heatmap", PreviewType::SLOPE_ELEVATION_HEATMAP},
};

/**
 * @brief Preview class, generic node preview widget (can be used for inputs and outputs).
 */
class DataPreview : public QLabel
{
public:
  DataPreview() = default;

  DataPreview(gngui::NodeProxy *p_proxy_node);

public Q_SLOTS:
  void update_image();

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

private:
  gngui::NodeProxy *p_proxy_node;
  int               preview_port_index;
  PreviewType       preview_type = PreviewType::GRAYSCALE;
};

} // namespace hesiod