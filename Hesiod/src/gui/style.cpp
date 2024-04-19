/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QFont>

#include <QtNodes/ConnectionStyle>

#include "hesiod/gui/style.hpp"

namespace hesiod
{

void set_style_qtapp(QApplication &app)
{
  QFont font("Roboto");
  font.setPointSize(10);
  app.setFont(font);

  // QFont defaultFont = app.font();
  // defaultFont.setPointSize(defaultFont.pointSize() - 2);
  // app.setFont(defaultFont);

  const std::string style_sheet =
#include "darkstyle.css"
      ;

  app.setStyleSheet(style_sheet.c_str());
}

void set_style_qtnodes()
{
  QtNodes::ConnectionStyle::setConnectionStyle(
      R"(
  {
  "GraphicsViewStyle": {
    "BackgroundColor": [255, 0, 0],
    "FineGridColor": [68, 71, 90],
    "CoarseGridColor": [88, 91, 45]
    },
  "ConnectionStyle": {
    "ConstructionColor": "gray",
    "NormalColor": "darkcyan",
    "SelectedColor": [100, 100, 100],
    "SelectedHaloColor": "orange",
    "HoveredColor": "lightcyan",

    "LineWidth": 1.5,
    "ConstructionLineWidth": 3.0,
    "PointDiameter": 10.0,

    "UseDataDefinedColors": true
  }

}
  )");
}

} // namespace hesiod
