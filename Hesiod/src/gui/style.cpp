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

  app.setStyleSheet(R"(
* {
    font-family: Roboto, sans-serif;
}

QSlider::groove:horizontal { 
	background: #D8DEE9;
	height: 24px; 
	border-radius: 4px;
}

QSlider::handle:horizontal { 
	background-color: #81A1C1; 
	width: 24px; 
	height: 24px; 
	border-radius: 8px;
}

QSlider::groove:vertical { 
	background: #D8DEE9;
	width: 24px; 
	border-radius: 4px;
}

QSlider::handle:vertical { 
	background-color: #81A1C1; 
	width: 24px; 
	height: 24px; 
	border-radius: 8px;
}

)");
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
